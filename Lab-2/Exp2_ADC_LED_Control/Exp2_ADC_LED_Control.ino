/*
 * Exp2_ADC_LED_CONTROL.ino
 * 
 * Aim: With the use of LDR, setting threshold for turning the LED On or Off
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 30-06-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * Red LED    -> PH3          -> Arduino pin 6
 * LDR Pin    -> PF0 (ADC[0]) -> Arduino pin A0
 * 
 * Note: All the LEDs are active low.
 */

// macro definitions
#define ledPin     6
#define SensorPin  A0

void setup() {
    Serial.begin(9600);
    // set RGB LED Pin as Output

    // set LDR Pin as Input

    // turn OFF the LED Pin initially
    
}

void loop() {
    
    // Getting the values from the Sensor 


    // Write the logic for setting threshold for making led On or Off


}