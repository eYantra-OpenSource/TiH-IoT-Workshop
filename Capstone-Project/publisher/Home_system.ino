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

// include Arduino DHT library
#include <DHT.h>


// macro definitions
#define SensorPin  A0

#define DHT_PIN     25      // pin where DHT11 Data is connected
#define DHT_TYPE    DHT11   // type of DHT sensor

// configure the sensor
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(9600);

    // set LDR Pin as Input
    pinMode(SensorPin, INPUT);
    
    // initialize the sensor here
    dht.begin();
}

void loop() {

    delay(1000); // Delays 1 seconds, as the DHT11 sampling rate is 1Hz
  
    // reading light, temperature or humidity takes about 250 ms
    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Getting the values from the Sensor 
    int ldr = analogRead(SensorPin);

    // Printing the results on the serial monitor
    Serial.print(ldr);
    Serial.print("|");
    Serial.print(t);
    Serial.print("|");
    Serial.println(h);

}
