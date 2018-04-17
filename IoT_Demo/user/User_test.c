#include "user_test.h"


////////////////////////////////////////////////////////////////////JSON
LOCAL int ICACHE_FLASH_ATTR  
dht11_get(struct jsontree_context *js_ctx)  
{  
    const char *path = jsontree_path_name(js_ctx, js_ctx->depth - 1);  
    char string[32];  
  
    if (os_strncmp(path, "temp", 4) == 0)  
    {  
        //os_sprintf(string, "%d\n",temperture);  
        os_sprintf(string,"25");  
				os_printf("temp\n");
    }  
    else if(os_strncmp(path, "hum", 3) == 0)  
    {  
        os_sprintf(string, "60");  
				os_printf("hum\n");
       // os_sprintf(string,"40");  
    }  
  
    jsontree_write_string(js_ctx, string);  
  
    return 0;  
}  
  
LOCAL struct jsontree_callback dht11_callback =  
    JSONTREE_CALLBACK(dht11_get, NULL);  
  
JSONTREE_OBJECT(get_dht11,  
                JSONTREE_PAIR("temp", &dht11_callback),  
        JSONTREE_PAIR("hum", &dht11_callback));  
JSONTREE_OBJECT(DHT11JSON,  
                JSONTREE_PAIR("dht11", &get_dht11));  
  
//返回DHT11数据的json格式  
char* ICACHE_FLASH_ATTR  
get_dht11_json(void)  
{  
    static char dht11_buf[64];  
    os_memset(dht11_buf,0,64);      //清空  
    json_ws_send((struct jsontree_value *)&DHT11JSON, "dht11", dht11_buf);  
    return dht11_buf;  
}  
/////////////////////////////////////////////////////////////////////////////



#define ESP_DEBUG		1

#ifdef ESP_DEBUG
#define ESP_DBG os_printf
#else
#define ESP_DBG
#endif
LOCAL struct espconn test_conn;//定义云平台参数
LOCAL struct _esp_tcp test_tcp;

int recon_count;

LOCAL void ICACHE_FLASH_ATTR
platform_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
    char *pstr = NULL;
    LOCAL char pbuffer[1024 * 2] = {0};
    struct espconn *pespconn = arg;

    ESP_DBG("platform_recv_cb %s\n", pusrdata);
}

void ICACHE_FLASH_ATTR
platform_discon_cb(void *arg)
{
	ESP_DBG("platform_discon_cb\n");
}

void ICACHE_FLASH_ATTR
platform_sent_cb(void *arg)
{
	ESP_DBG("platform_sent_cb\n");
}

LOCAL void ICACHE_FLASH_ATTR
platform_recon_cb(void *arg, sint8 err)
{
    struct espconn *pespconn = (struct espconn *)arg;

    ESP_DBG("platform_recon_cb\n");//需要自己再次连接
    //os_timer_disarm(&beacon_timer);

    if (++recon_count == 5)
    {
    	os_printf("connect fail\r\n");
		return;
    }
	espconn_connect(&test_conn);
	/*
    os_timer_disarm(&client_timer);
    os_timer_setfn(&client_timer, (os_timer_func_t *)user_esp_platform_reconnect, pespconn);
    os_timer_arm(&client_timer, 1000, 0);
    */
}

LOCAL void ICACHE_FLASH_ATTR
platform_connect_cb(void *arg)
{
    struct espconn *pespconn = arg;

    ESP_DBG("platform_connect_cb\n");
    if (wifi_get_opmode() ==  STATIONAP_MODE )
    {
        wifi_set_opmode(STATION_MODE);
    }


    recon_count = 0;
    espconn_regist_recvcb(pespconn, platform_recv_cb);
    espconn_regist_sentcb(pespconn, platform_sent_cb);
	espconn_regist_disconcb(&test_conn, platform_discon_cb);
    //user_esp_platform_sent(pespconn);
}
os_timer_t connect_t;
void ICACHE_FLASH_ATTR 
connect_fn(void *agr)
{
	struct espconn *conn = agr;
	espconn_connect(conn);
}

void ICACHE_FLASH_ATTR
platform_init(void)
{
	test_conn.proto.tcp = &test_tcp;
	test_conn.type = ESPCONN_TCP;
	test_conn.state = ESPCONN_NONE;

	const char esp_server_ip[4] = {115,29,202,58};
	os_memcpy(test_conn.proto.tcp->remote_ip, esp_server_ip, 4);
	test_conn.proto.tcp->local_port = espconn_port();

	test_conn.proto.tcp->remote_port = 8000;
	
	espconn_regist_connectcb(&test_conn, platform_connect_cb);
	espconn_regist_reconcb(&test_conn, platform_recon_cb);// once only
	os_timer_disarm(&connect_t);
	os_timer_setfn(&connect_t,(os_timer_func_t *)connect_fn,&test_conn);
	os_timer_arm(&connect_t,5000,0);
//	espconn_connect(&test_conn);
}



void User_test(void)
{

	os_printf("User Test Begain!\n");
	
 //  os_printf("%s\n",get_dht11_json());  
	platform_init();

}


