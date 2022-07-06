/*
 * Exp1a_ATmega_UART.ino
 * 
 * Aim: Enable UART0 communication from ATmega2560 to ESP32.
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 05-07-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * ATmega2560 TX0  ->   ESP32 RX1
 * ATmega2560 RX0  ->   ESP32 TX1
 */

void setup() {
  // enable UART0
  Serial.begin(9600);
}

void loop() {

  // send data to ESP32 via UART0
  Serial.print("Tx from ATmega 2560> Hello ESP32!\n");
  delay(1000);

}
