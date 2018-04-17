/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "gpio.h"
#include "user_interface.h"

#include "driver/key.h"
#include "User_plug.h"
#include "smartlink.h"
volatile unsigned char key_status = 0;
volatile unsigned int  key_press_time = 0;

void ICACHE_FLASH_ATTR
key_service(void *agr)
{
    static unsigned char key_release_time = 0;
    static unsigned char key_click = 0;
    static unsigned char Continue = 0;
    unsigned char ReadData = 0;
    unsigned char Release;
    unsigned char Trigger;
    if(!GPIO_INPUT_GET(GPIO_ID_PIN(0)))
    {
        ReadData = 1;
    }
    Trigger  = ReadData & (ReadData ^ Continue);
    Release  = (ReadData ^ Trigger ^ Continue);
    Continue = ReadData;
    if (Trigger) key_press_time = 0; // 或者在释放时清零
    if (Continue)
    {
        if (key_press_time++ > press_time_min)
        {
            key_status = 254; // 长按操作
        }
    }
    if (Release)
    {
        if ((key_press_time > click_time_min) &&
            (key_press_time <= click_time_max)) // 短按操作, 介于30ms到500ms
        {
            key_click++;
        }
        else if ((key_press_time > click_time_max) &&
                 (key_press_time <= press_time_min))
        {
            key_click = 0;
        }
        else if (key_press_time > press_time_min) // 长按释放
        {
            key_status = 255;
        }
        key_release_time = 0;
    }
    if (key_release_time++ > click_time_max)
    {
        if (key_click)
        {
            key_status = key_click;
            key_click = 0;
        }
    }
    switch(key_status)
    {
        case 1:
#if PLUG_DEVICE
           user_plug_short_press();
#endif
          // user_plug_set_status(0);
          // GPIO_OUTPUT_SET(PLUG_RELAY_LED_IO_NUM, 0);
            break;
			case 2: SmartLinkBegin();//SmartLink();
			break;
		
        case 254:
#if PLUG_DEVICE
          user_plug_long_press();
#endif
           //GPIO_OUTPUT_SET(PLUG_RELAY_LED_IO_NUM, 1);
            break;
        default :
            break;
    }
    key_status = 0;
}

os_timer_t key_tim;
/******************************************************************************
 * FunctionName : key_init
 * Description  : init keys
 * Parameters   : key_param *keys - keys parameter, which inited by key_init_single
 * Returns      : none
*******************************************************************************/
void ICACHE_FLASH_ATTR
key_init(void)
{
	//PIN_FUNC_SELECT(PLUG_KEY_0_IO_MUX, PLUG_KEY_0_IO_FUNC);
    key_tim.timer_func = key_service;
    os_timer_disarm(&key_tim);
    os_timer_setfn(&key_tim, (os_timer_func_t *)key_service, NULL);
    os_timer_arm(&key_tim, 10, 1);
}
