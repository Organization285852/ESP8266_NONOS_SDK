#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "mem.h"


#include "user_interface.h"
#include "airkiss.h"
#include "User_config.h"
#include "smartlink.h"





#define DEVICE_TYPE         "gh_9e2cff3dfa51" //wechat public number
#define DEVICE_ID           "122475" //model ID

#define DEFAULT_LAN_PORT    12476

LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv;

uint8_t  lan_buf[200];
uint16_t lan_buf_len;
uint8    udp_sent_cnt = 0;

const airkiss_config_t akconf =
{
    (airkiss_memset_fn)&memset,
    (airkiss_memcpy_fn)&memcpy,
    (airkiss_memcmp_fn)&memcmp,
    0,
};

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_time_callback(void)
{
    uint16 i;
    airkiss_lan_ret_t ret;
    /*
    if ((udp_sent_cnt++) >30) {
        udp_sent_cnt = 0;
        os_timer_disarm(&ssdp_time_serv);//s
        //return;
    }

    ssdp_udp.remote_port = DEFAULT_LAN_PORT;
    ssdp_udp.remote_ip[0] = 255;
    ssdp_udp.remote_ip[1] = 255;
    ssdp_udp.remote_ip[2] = 255;
    ssdp_udp.remote_ip[3] = 255;
    lan_buf_len = sizeof(lan_buf);
    ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
        DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
    if (ret != AIRKISS_LAN_PAKE_READY) {
        os_printf("Pack lan packet error!");
        return;
    }

    ret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
    if (ret != 0) {
        os_printf("UDP send error!");
    }
    os_printf("Finish send notify!\n");
    */
}

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_recv_callbk(void *arg, char *pdata, unsigned short len)
{
    uint16 i;
    remot_info* pcon_info = NULL;
    os_printf("\r\n\r\n");
    for (i = 0; i < len; i++)
        os_printf("%c", pdata[i]);
    os_printf("\r\n\r\n");
		/*
    airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
    airkiss_lan_ret_t packret;

    switch (ret){
    case AIRKISS_LAN_SSDP_REQ:
        espconn_get_connection_info(&pssdpudpconn, &pcon_info, 0);
        os_printf("remote ip: %d.%d.%d.%d \r\n",pcon_info->remote_ip[0],pcon_info->remote_ip[1],
                                                pcon_info->remote_ip[2],pcon_info->remote_ip[3]);
        os_printf("remote port: %d \r\n",pcon_info->remote_port);

        pssdpudpconn.proto.udp->remote_port = pcon_info->remote_port;
        os_memcpy(pssdpudpconn.proto.udp->remote_ip,pcon_info->remote_ip,4);
        ssdp_udp.remote_port = DEFAULT_LAN_PORT;

        lan_buf_len = sizeof(lan_buf);
        packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
            DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);

        if (packret != AIRKISS_LAN_PAKE_READY) {
            os_printf("Pack lan packet error!");
            return;
        }

        packret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
        if (packret != 0) {
            os_printf("LAN UDP Send err!");
        }

        break;
    default:
        os_printf("Pack is not ssdq req!%d\r\n",ret);
        break;
    }
    */
}

void ICACHE_FLASH_ATTR
airkiss_start_discover(void)
{
    ssdp_udp.local_port = DEFAULT_LAN_PORT;
    pssdpudpconn.type = ESPCONN_UDP;
    pssdpudpconn.proto.udp = &(ssdp_udp);
    espconn_regist_recvcb(&pssdpudpconn, airkiss_wifilan_recv_callbk);
    espconn_create(&pssdpudpconn);

    os_timer_disarm(&ssdp_time_serv);
    os_timer_setfn(&ssdp_time_serv,
                   (os_timer_func_t *)airkiss_wifilan_time_callback, NULL);
    os_timer_arm(&ssdp_time_serv, 1000, 1);//1s
}


void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)
{
    switch(status)
    {
        case SC_STATUS_WAIT:
            os_printf("SC_STATUS_WAIT\n");
            break;
        case SC_STATUS_FIND_CHANNEL:
            os_printf("SC_STATUS_FIND_CHANNEL\n");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
            os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
            sc_type *type = pdata;
            if (*type == SC_TYPE_ESPTOUCH)
            {
                os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
            }
            else
            {
                os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
            }
            break;
        case SC_STATUS_LINK:
            os_printf("SC_STATUS_LINK\n");
            struct station_config *sta_conf = pdata;
            system_param_save_with_protect(LAST_AP_START_SEC, sta_conf,
                                           sizeof(struct station_config));
            wifi_station_set_config(sta_conf);
            wifi_station_disconnect();
            wifi_station_connect();
            break;
        case SC_STATUS_LINK_OVER:
            os_printf("SC_STATUS_LINK_OVER\n");
            if (pdata != NULL)
            {
                //SC_TYPE_ESPTOUCH
                uint8 phone_ip[4] = {0};

                os_memcpy(phone_ip, (uint8*)pdata, 4);
                os_printf("Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2],
                          phone_ip[3]);
            }
            else
            {
                //SC_TYPE_AIRKISS - support airkiss v2.0
                airkiss_start_discover();
            }
            smartconfig_stop();
            break;
    }

}

os_timer_t scan_t;
static void ICACHE_FLASH_ATTR
scan_done(void *arg, STATUS status)
{
    int ret;
    struct station_config sta_conf;
    system_param_load(LAST_AP_START_SEC, 0, &sta_conf,
                      sizeof(sta_conf)); //last link's AP
    Debug_printf("Last Link AP's SSID:%s\r\n", sta_conf.ssid);

    if (status == OK)
    {

        struct bss_info *bss_link = (struct bss_info *)arg;

        while (bss_link != NULL)
        {
            if(os_strcmp(sta_conf.ssid, bss_link->ssid) == 0)
            {
                Debug_printf("Link last AP\r\n");
                wifi_station_set_config(&sta_conf);
                wifi_station_disconnect();
                wifi_station_connect();
            }
            bss_link = bss_link->next.stqe_next;
        }
		os_timer_disarm(&scan_t);
    }
    else
    {
        os_printf("err, scan status %d\n", status);
    }

}



void station_scan(void *agr)
{
	 if(wifi_get_opmode() == STATION_MODE)
    {
  		Debug_printf("wifi_station_scan\r\n");
  	 	wifi_station_scan(NULL, scan_done);
    }
}
void ICACHE_FLASH_ATTR
SmartLink(void)
{
	wifi_set_opmode(STATION_MODE);	
    //通过AIRKISS连接
    smartconfig_set_type(
        SC_TYPE_AIRKISS); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
    smartconfig_start(smartconfig_done);
}

		
void ICACHE_FLASH_ATTR
SmartLinkBegin(void)
{
	wifi_set_opmode(STATION_MODE);	
	scan_t.timer_func = station_scan;
	os_timer_disarm(&scan_t);
	os_timer_setfn(&scan_t,(os_timer_func_t *)station_scan,NULL);
	os_timer_arm(&scan_t,3000,1);
	/*
    //通过AIRKISS连接
    smartconfig_set_type(
        SC_TYPE_AIRKISS); //SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
    smartconfig_start(smartconfig_done);*/
}


