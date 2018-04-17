
#include "driver/RF1527.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h"
#define SYS_H_MIN       250//300us
#define SYS_H_MAX       380//

#define SYS_L_MIN       8200//8300
#define SYS_L_MAX       8400

#define BIT1_L_VAULE   420//320us

#define T_MIN                   700//1080us
#define T_MAX                   1100

#define END_L_MIN   50000 //77000us
#define END_H_MAX   1100
#define END_H_MIN   200 //1000us

unsigned int t, t_last = 0, TL, TH, temp;
unsigned int data_1527, keys;
u8 bits = 0, sys_ok = 0;
void rf1527_intr_handler(void  *agr)
{
    static u8 status = 0;
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(RFIN_IO_NUM));
    t = system_get_time();
    if(GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_IO_NUM)))
    {
        TL = t - t_last;
		t_last = system_get_time();
        os_printf("%d %d\n",TH,TL);
        if((TH >= SYS_H_MIN) && (TH <= SYS_H_MAX) && (TL >= SYS_L_MIN) &&
           (TL <= SYS_L_MAX))//数据同步
        {
            data_1527 = 0;
            bits = 0;
            sys_ok = 1;
            //os_printf("sys_ok\n");
            return;
        }

        if((TL >= END_L_MIN) && (TH >= END_H_MIN) && (TH <= END_H_MAX))
        {
            os_printf("key is %x\n",  keys);
            return;
        }

        if(sys_ok == 1)
        {
            temp = TH + TL;
            if((temp > T_MAX) || (temp < T_MIN))//滤除
            {
                sys_ok = 0;
                return;
            }
            data_1527 <<= 1;
            if(TL < BIT1_L_VAULE)
            {
                data_1527++;
            }
            bits++;
            if(bits >= 24)
            {
                sys_ok = 0;
                //os_printf("key is %d\n", data_1527);
                keys = data_1527;
            }
        }
    }
    else
    {
        TH = t - t_last;
		t_last = system_get_time();
    }
}

#define SYS_MIN       550//750us
#define SYS_MAX       950//
u8 ir_buf[6],sum;
float tt;
int ttt;
void ir_intr_handler(void  *agr)
{
    static u8 status = 0, sys_cnt = 0;
    //clear interrupt status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(RFIN_IO_NUM));
    t = system_get_time();
    if(GPIO_INPUT_GET(GPIO_ID_PIN(RFIN_IO_NUM)))
    {
        TL = t - t_last;
        t_last = t;
        //os_printf("%d %d\n", TH, TL);
		
        if((TH >= SYS_MIN) && (TH <= SYS_MAX) && (TL >= SYS_MIN) &&
           (TL <= SYS_MAX))//数据同步
        {
            sys_cnt ++;   
            if(sys_cnt >= 4)
            {
                sys_cnt = 0;
                sys_ok = 1;
				//os_printf("bits:%d ",bits);
				if(bits==42)
				{
					sum = ir_buf[0]+ir_buf[1]+ir_buf[2]+ir_buf[3];
					if(sum==ir_buf[4])
					{
						tt = (ir_buf[1]&0x0f)*256 +  ir_buf[2];
						tt -=900;
						tt = tt*0.1;
						tt -=32;
						tt = tt/1.8;
						ttt = tt*10;
					//	os_printf("HEX is %x,%x,%x,%x,%x,%x ", ir_buf[0],ir_buf[1],ir_buf[2],ir_buf[3],ir_buf[4],ir_buf[5]);
					//	os_printf("DEC is %d,%d,%d,%d,%d,%d\n", ir_buf[0],ir_buf[1],ir_buf[2],ir_buf[3],ir_buf[4],ir_buf[5]);
					//	os_printf("%d\n",ttt);
						os_memset(ir_buf, 0, sizeof(ir_buf));

				
					}
				}
				bits = 0;
            }
            return;
        }
        else
        {
            sys_cnt = 0;
        }
   /*
        if((TL >= END_L_MIN)&& (TH >= END_H_MIN) && (TH <= END_H_MAX))
        {
            os_printf("key is %x\n",  keys);
            return;
        }
      */
        if(sys_ok == 1)
        {
            temp = TH + TL;
            if((temp > 800) || (temp < 600))//滤除
            {
                sys_ok = 0;
                return;
            }
            ir_buf[bits/8]<<= 1;
            if(TH > TL)
            {
               ir_buf[bits/8]++;
            }
            bits++;
			if(bits>=48)
			{
				 sys_ok = 0;
               return;
			}
			/*			
            if(bits >= 41)
             {
                 sys_ok = 0;
                 os_printf("HEX is %x%x%x%x%x%x\n", ir_buf[0],ir_buf[1],ir_buf[2],ir_buf[3],ir_buf[4],ir_buf[5]);
                // keys = data_1527;
             }
             */
        }
    }
    else
    {
        TH = t - t_last;
        t_last = t;
    }

}



void ICACHE_FLASH_ATTR
rf1527_init(void)
{

    ETS_GPIO_INTR_ATTACH(ir_intr_handler, NULL);

    ETS_GPIO_INTR_DISABLE();

    PIN_FUNC_SELECT(RFIN_IO_MUX, RFIN_IO_FUNC);

    gpio_output_set(0, 0, 0, GPIO_ID_PIN(RFIN_IO_NUM));

    gpio_register_set(GPIO_PIN_ADDR(RFIN_IO_NUM),
                      GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
                      | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
                      | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));

    //clear gpiox status
    GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(RFIN_IO_NUM));

    //enable interrupt
    gpio_pin_intr_state_set(GPIO_ID_PIN(RFIN_IO_NUM),
                            GPIO_PIN_INTR_ANYEDGE);

    ETS_GPIO_INTR_ENABLE();
}






