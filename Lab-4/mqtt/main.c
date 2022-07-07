//------------------------------------------------------------------------------
#include "ey_iot.h"
#include "ey_iot_config.h"

//------------------------------------------------------------------------------
#define SUB_TOPIC   "/tih/chat/group"
#define PUB_TOPIC   "/tih/chat/my"
#define DELAY_SEC	60

//------------------------------------------------------------------------------
void init_callback(void);
void my_wifi_start(void);
void my_wifi_disconnect(void);
void my_mqtt_rx(void);
void my_mqtt_disconnect(void);

//------------------------------------------------------------------------------
void app_main(void){

	init_callback();
	ey_init_nvs();
    ey_init_wifi_sta(EY_CONFIG_WIFI_SSID, EY_CONFIG_WIFI_PASS);
    ey_mqtt_start();
    ey_mqtt_subscribe(SUB_TOPIC, 0);
}


//------------------------------------------------------------------------------
void init_callback(void){
	
	ey_register_callback(EY_WIFI_EVENT_STA_START, my_wifi_start);
	ey_register_callback(EY_WIFI_EVENT_STA_DISCONNECTED, my_wifi_disconnect);
	ey_register_callback(EY_MQTT_EVENT_DATA_RX, my_mqtt_rx);

}


void my_wifi_start() { 
	printf("USER: Wi-Fi Start\n");
}


void my_wifi_disconnect() { 
	printf("USER: Wi-Fi Disconnect\n");
	ey_wifi_auto_reconnect();
}


void my_mqtt_rx(){
	printf("SUB_TOPIC: %s\n", ey_mqtt_sub_topic);
    printf("SUB_DATA: %s\n", ey_mqtt_sub_data);

    ey_mqtt_publish(PUB_TOPIC, "I am Lohit", 0);
    printf("PUB_DATA: %s\n", "Sent message");
 
}

void my_mqtt_disconnect(){
	printf("USER: MQTT Disconnect\n");
	ey_wifi_auto_reconnect();
}
