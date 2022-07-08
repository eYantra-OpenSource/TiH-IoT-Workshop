/* 
TITLE	: UART Communication: ESP32 & ATmega 2560
DATE	: 2019/11/12
AUTHOR	: e-Yantra Team
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>


#include "driver/uart.h"
#include "driver/gpio.h"

#define ECHO_TEST_TXD  (GPIO_NUM_32)            // Connected to AVR Rx-0
#define ECHO_TEST_RXD  (GPIO_NUM_33)            // Connected to AVR Tx-0
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

//------------------------------------------------------------------------------
#include "ey_iot.c"
#include "ey_iot_config.h"

//------------------------------------------------------------------------------
#define FAN_TOPIC   "ey/fan"
#define LED_TOPIC   "ey/light"

//------------------------------------------------------------------------------
void init_callback(void);
void my_wifi_start(void);
void my_wifi_disconnect(void);
void my_mqtt_rx(void);
void my_mqtt_disconnect(void);
void my_ota_success(void);
void my_ota_fail(void);
void my_task_alert_msg_rcv(void *p);

//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
void init_callback(void){
	
	ey_register_callback(EY_WIFI_EVENT_STA_START, my_wifi_start);
	ey_register_callback(EY_WIFI_EVENT_STA_DISCONNECTED, my_wifi_disconnect);

	ey_register_callback(EY_MQTT_EVENT_DATA_RX, my_mqtt_rx);

	ey_register_callback(EY_OTA_EVENT_SUCCESS, my_ota_success);
	ey_register_callback(EY_OTA_EVENT_FAIL, my_ota_fail);
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
}



void my_mqtt_disconnect(){
	printf("USER: MQTT Disconnect\n");
	ey_wifi_auto_reconnect();
}


void my_ota_success(){
	printf("USER: OTA Success\n");
	// ey_mqtt_publish(PUB_TOPIC, "ota-success", 0);
	vTaskDelay(2000 / portTICK_PERIOD_MS);
	esp_restart();
}

void my_ota_fail(){
	printf("USER: OTA Fail\n");
}

void init_uart(void) {
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
}

void my_task_alert_msg_rcv(void *p)
{
    long unsigned int start_time = 0, current_time = 0;

    start_time = ey_sntp_get_epoch();

    while(1)
    {
        current_time = ey_sntp_get_epoch();

        if((current_time - start_time) >= 10) {

            if ( ey_mqtt_sub_data[0] != '\0' ) {
                printf("Write to UART: %s\n", ey_mqtt_sub_data);
                uart_write_bytes(UART_NUM_1, ey_mqtt_sub_data, strlen(ey_mqtt_sub_data));
                ey_mqtt_sub_data[0] = '\0';
            }

        }
    }
}


void app_main()
{
    init_callback();
    init_uart();
    ey_init_nvs();
    ey_init_wifi_sta(EY_CONFIG_WIFI_SSID, EY_CONFIG_WIFI_PASS);
    ey_init_ntp();
    ey_mqtt_start();

    ey_mqtt_subscribe(FAN_TOPIC, 0);
    ey_mqtt_subscribe(LED_TOPIC, 0);

    xTaskCreate(&my_task_alert_msg_rcv, "my_task_alert_msg_rcv", 8120, NULL, 10, NULL);
}

