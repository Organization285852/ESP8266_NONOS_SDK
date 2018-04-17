#ifndef __MQTT_CONFIG_H__
#define __MQTT_CONFIG_H__

#define CFG_HOLDER	0x00FF55A5	/* Change this value to load default configurations */
#define CFG_LOCATION	0x79	/* Please don't change or if you know what you doing */
#define MQTT_SSL_ENABLE

/*DEFAULT CONFIGURATIONS*/

#define MQTT_HOST			"m14.cloudmqtt.com" //or "mqtt.yourdomain.com"
#define MQTT_PORT			17429
#define MQTT_BUF_SIZE		1024
#define MQTT_KEEPALIVE		120	 /*second*/

#define MQTT_CLIENT_ID		"DVES_%08X"
#define MQTT_USER			"mbivhzli"
#define MQTT_PASS			"d5pXYDXe_grl"

#define STA_SSID "howseen"
#define STA_PASS "howseen666"
#define STA_TYPE AUTH_WPA2_PSK

#define MQTT_PLUG_SWITCH_TOPIC			"zq1122wg/switch/"
#define MQTT_PLUG_STATE_TOPIC				"zq1122wg/state/"
#define MQTT_RF433_SEND_TOPIC				"zq1122wg/send/rf433/"
#define MQTT_RF433_RESIVE_TOPIC			"zq1122wg/resive/rf433/"

#define MQTT_RECONNECT_TIMEOUT 	5	/*second*/

#define DEFAULT_SECURITY	0
#define QUEUE_BUFFER_SIZE		 		2048

#define PROTOCOL_NAMEv31	/*MQTT version 3.1 compatible with Mosquitto v0.15*/
//PROTOCOL_NAMEv311			/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#endif // __MQTT_CONFIG_H__
