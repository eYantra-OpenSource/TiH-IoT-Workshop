/*
 * Exp3_DHT11.ino
 * 
 * Aim: Monitor the temperature and humidity around with DHT11.
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 30-06-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * DHT11 Data   -> PA3  -> Arduino pin 25
 */

// include Arduino DHT library
#include <DHT.h>

// macro definitions
#define DHT_PIN     25      // pin where DHT11 Data is connected
#define DHT_TYPE    DHT11   // type of DHT sensor

// configure the sensor
DHT dht(DHT_PIN, DHT_TYPE);

void setup() {

  // enable the serial port
  Serial.begin(9600);

  // initialize the sensor
  dht.begin();
}

void loop() {

  delay(1000); // Delays 1 seconds, as the DHT11 sampling rate is 1Hz
  
  // reading temperature or humidity takes about 250 ms
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Printing the results on the serial monitor
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.print(" % ");
  Serial.print("    Temperature = ");
  Serial.print(t);
  Serial.println(" *C");
  
}
