/*
 * Exp1_ADC_BASIC.ino
 * 
 * Aim: Print the values of ADC in Serial Monitor
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 30-06-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * LDR Pin    -> PF0 (ADC[0]) -> Arduino A0
 * 
 * Note: All the LEDs are active low.
 */

// macro definitions
#define SensorPin  A0

void setup() {
    Serial.begin(9600);
    // set LDR Pin as Input
    pinMode(SensorPin, INPUT);
}

void loop() {
    // Getting the values from the Sensor 
    int SensorVal = analogRead(SensorPin);

    // Printing the values received from Sensor in serial monitor
    Serial.println(SensorVal);
}