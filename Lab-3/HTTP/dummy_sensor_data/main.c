//------------------------------------------------------------------------------
#include "ey_iot.c"
#include "ey_iot_config.h"


//------------------------------------------------------------------------------
void init_callback(void);
void my_wifi_start(void);
void my_wifi_disconnect(void);
void my_task_sensor_data_logging(void *p);

//------------------------------------------------------------------------------
void app_main(void){

	init_callback();
    ey_init_nvs();
    ey_init_wifi_sta(EY_CONFIG_WIFI_SSID, EY_CONFIG_WIFI_PASS);
    xTaskCreate(&my_task_sensor_data_logging, "my_task_sensor_data_logging", 8120, NULL, 10, NULL);
}


//------------------------------------------------------------------------------
void init_callback(void){
	
	ey_register_callback(EY_WIFI_EVENT_STA_START, my_wifi_start);
	ey_register_callback(EY_WIFI_EVENT_STA_DISCONNECTED, my_wifi_disconnect);
}


void my_wifi_start() { 
	printf("USER: Wi-Fi Start\n");
}


void my_wifi_disconnect() { 
	printf("USER: Wi-Fi Disconnect\n");
	ey_wifi_auto_reconnect();
}

void my_task_sensor_data_logging(void *p){
	
    char sensor_data = 99;
    
    while(1){
		
			// Print Sensor
			printf(WHT "Sensor: " RESET); printf(YEL "%d\n" RESET,sensor_data);
        	ey_push_sensor_data(GOOGLE_SHEET, sensor_data);
			printf("\n");
 
        	sensor_data--;
        	if(sensor_data == 0){
        		sensor_data = 99;
        	}
    }
}
//------------------------------------------------------------------------------