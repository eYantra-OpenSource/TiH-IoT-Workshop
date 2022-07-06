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

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"

// macro definitions
#define ECHO_TEST_TXD  (GPIO_NUM_32)            // Connected to AVR Rx-0
#define ECHO_TEST_RXD  (GPIO_NUM_33)            // Connected to AVR Tx-0
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (150)

// function to receive data on UART1 from ATmega
void rcv_task(void *arg)
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
    uint8_t* data = (uint8_t *) malloc(BUF_SIZE);

    while (1)
    {
        // read data from the UART
        int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_PERIOD_MS);

        if (len > 0) {
            data[len] = '\0';
            printf("Rx from ATmega: %s", (char *) data);
            ESP_LOGI("UART TEST", "Recv str: %s", (char *) data);
        }
    }
}

void app_main(void)
{
    xTaskCreate(rcv_task, "uart_rcv_task", 8120, NULL, 10, NULL);
}