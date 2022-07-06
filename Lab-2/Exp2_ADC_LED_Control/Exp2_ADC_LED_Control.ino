/*
 * Exp2_ADC_LED_CONTROL.ino
 * 
 * Aim: With the use of ldr, setting threshold for turning the led on or off
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 30-06-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * Red LED    -> PH3          -> Arduino pin 6
 * LDR Pin    -> PF0 (ADC[0]) -> Arduino A0
 * 
 * Note: All the LEDs are active low.
 */

// macro definitions
#define ledPin  6
#define SensorPin  A0

void setup() {
    Serial.begin(9600);
    // set RGB LED Pin as Output
    pinMode(ledPin, OUTPUT);
    // set LDR Pin as Input
    pinMode(SensorPin, INPUT);

    // turn OFF the LED Pin initially
    digitalWrite(ledPin, HIGH);
}

void loop() {
    // Getting the values from the Sensor 
    int SensorVal = analogRead(SensorPin);
    if (SensorVal <= 200) {
        digitalWrite(ledPin, LOW);
        Serial.print("Its DARK, Turn on the LED : ");
        Serial.println(SensorVal);
    } 
    else {
        digitalWrite(ledPin, HIGH);
        Serial.print("Its BRIGHT, Turn off the LED : ");
        Serial.println(SensorVal);
    }
}