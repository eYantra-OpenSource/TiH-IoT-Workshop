//------------------------------------------------------------------------------
#include "ey_iot.c"
#include "ey_iot_config.h"

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

//------------------------------------------------------------------------------
#define SUB_TOPIC   "/tih/chat/group"
#define PUB_TOPIC_A   "ey/fan"
#define PUB_TOPIC_B   "ey/light"
#define DELAY_SEC	60

// macro definitions
#define ECHO_TEST_TXD  (GPIO_NUM_32)            // Connected to AVR Rx-0
#define ECHO_TEST_RXD  (GPIO_NUM_33)            // Connected to AVR Tx-0
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (150)

//------------------------------------------------------------------------------
// Global variables, sensor data and actuator data
char sensor_data[18], Light[3], Fan[12];

//------------------------------------------------------------------------------
void init_callback(void);
void my_wifi_start(void);
void my_wifi_disconnect(void);
void my_mqtt_rx(void);
void my_mqtt_disconnect(void);

void rcv_sensor_data(void *arg)
{
    // configure UART driver
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    
    // set the UART pins
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS));
    
    // install the UART driver and set UART to default config
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0));

    // create temporary buffer for incoming data
    uint8_t* sensor_data = (uint8_t *) malloc(BUF_SIZE);
    float sensor_light, sensor_temp, sensor_humid;

    while (1)
    {
        // read data from the UART
        int len = uart_read_bytes(UART_NUM_1, sensor_data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

        if (len > 0)
        {
            sensor_data[len] = '\0';
            //printf("Rx from ATmega: %s", (char *) sensor_data);
            //ESP_LOGI("UART TEST", "Recv str: %s", (char *) sensor_data);
            char *token = strtok((char *) sensor_data, "|");

            while( token != NULL )
            {
                sensor_light = atof(token);
                token = strtok(NULL, "|");

                sensor_temp = atof(token);
                token = strtok(NULL, "|");

                sensor_humid = atof(token);
                token = strtok(NULL, "|");

                printf("%0.2f | ", sensor_light);
                printf("%0.2f | ", sensor_temp);
                printf("%0.2f \n", sensor_humid);

                x_ey_push_sensor_data(GOOGLE_SHEET, 3, sensor_temp, sensor_humid, sensor_light);
                break;

                // char Header = alert_message[0];
                // char Footer = alert_message[1];
                // if (!strcmp(Header,'|')){
                //     ey_mqtt_publish(PUB_TOPIC_A, Footer, 0);
                // }
                // else{
                //     ey_mqtt_publish(PUB_TOPIC_B, Footer, 0);
                // }
            }
            if (alert_message[0] != '\0')
            {
                printf("alert rcvd: %s\n", alert_message);
                char *token1 = strtok((char *) alert_message, "|");

                while(token1 != NULL)
                {
                    char fan_msg[50];
                    strcpy(fan_msg, token1);
                    
                    token1 = strtok(NULL, "|");

                    char light_msg[50];
                    strcpy(light_msg, token1);

                    printf("%s | %s\n", fan_msg, light_msg);

                    ey_mqtt_publish(PUB_TOPIC_A, fan_msg, 0);
                    ey_mqtt_publish(PUB_TOPIC_B, light_msg, 0);

                    break;
                }
                alert_message[0] = '\0';
            }
        }
    }
}

void send_controls()
{
    char Header = alert_message[0];
    char Footer = alert_message[1];
    if (!strcmp(Header,'|')){
        ey_mqtt_publish(PUB_TOPIC_A, Footer, 0);
    }
    else{
        ey_mqtt_publish(PUB_TOPIC_B, Footer, 0);
    }
}
//------------------------------------------------------------------------------
void app_main(void){
	init_callback();
	ey_init_nvs();
    ey_init_wifi_sta(EY_CONFIG_WIFI_SSID, EY_CONFIG_WIFI_PASS);
    ey_mqtt_start();
    

    xTaskCreate(rcv_sensor_data, "uart_rcv_task_and_publish", 8120, NULL, 10, NULL);
    
    //ey_mqtt_subscribe(SUB_TOPIC, 0);
    
    // xTaskCreate(send_controls, "Sent_controls", 8120, NULL, 5, NULL);
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


void my_mqtt_rx(){                      //char publish_data){
	printf("SUB_TOPIC: %s\n", ey_mqtt_sub_topic);
    printf("SUB_DATA: %s\n", ey_mqtt_sub_data);
    if (!strcmp(ey_mqtt_sub_topic, "ey/fan")){
        ey_mqtt_publish("ey/fan", ey_mqtt_sub_data, 0);
    }
    else{
        ey_mqtt_publish("ey/light", ey_mqtt_sub_data, 0);
    }
 
    //ey_mqtt_publish(PUB_TOPIC_A, "I am a Publisher", 0);
    //char publish_text[] = (char);
    //printf("PUB_DATA: %s\n", publish_text);
 
}

void my_mqtt_disconnect(){
	printf("USER: MQTT Disconnect\n");
	ey_wifi_auto_reconnect();
}

/*
 * esp32_to_atmega
 * 
 * Aim: Enable UART 1 communication from ESP32 to ATmega2560.
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 05-07-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * ATmega2560 TX0	->	ESP32 RX1
 * ATmega2560 RX0	->	ESP32 TX1
 */




// function to receive data on UART1 from ATmega



   