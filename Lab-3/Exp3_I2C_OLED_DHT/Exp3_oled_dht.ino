/*
 * Exp4_OLED-DHT11.ino
 * 
 * Aim: Display DHT11 sensor values on OLED Display. 
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 01-07-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * OLED SDA     -> SDA (PD1)  -> Arduino pin 20
 * OLED SCL     -> SCL (PD0)  -> Arduino pin 21
 * DHT11 Data   -> PA3        -> Arduino pin 25
 */

// include Arduino DHT and SSH1106 OLED libraries
#include "DHT.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// macro definitions
#define DHT_PIN     25                // pin where DHT11 Data is connected
#define DHT_TYPE    DHT11             // type of DHT sensor

// configure the sensor
DHT dht(DHT_PIN,DHT_TYPE);

// variables for storing the sensor values
float humi;
float tempC;
float tempF;

// configure the OLED display
#define i2c_Address 0x3c              //initialize with the I2C addr 0x3C 
#define SCREEN_WIDTH 128              // OLED display width, in pixels
#define SCREEN_HEIGHT 64              // OLED display height, in pixels
#define OLED_RESET -1                 //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {

  // enable the serial port
  Serial.begin(9600);
  
  // initialize the sensor
  dht.begin();
  
  //For OLED I2C
  display.begin(i2c_Address, true);   // Address 0x3C default
  display.display(); //Display logo
  delay(1000); 
  display.clearDisplay();
}

void loop() {
 delay(1000);

 // Read data from the sensor
 humi = dht.readHumidity();
 tempC = dht.readTemperature();
 tempF = dht.readTemperature(true);

 // Display header (Humidity and Temperature) on OLED  
 oledDisplayHeader();
 
 // Display sensor values on OLED 
 oledDisplay(3,5,28,humi,"%");
 oledDisplay(2,82,16,tempC,"C");
 oledDisplay(2,82,44,tempF,"F");
 
 display.display(); 
 display.clearDisplay();
}

// Function to display header (Humidity and Temperature) on OLED
void oledDisplayHeader(){
 display.setTextSize(1);              
 display.setTextColor(SH110X_WHITE);
 display.setCursor(0, 0);
 display.print("Humidity");
 display.setCursor(60, 0);
 display.print("Temperature");
}

// Function to display sensor values on OLED
void oledDisplay(int size, int x,int y, float value, String unit){
 int charLen=9;
 int xo=x+charLen*3.2;
 int xunit=x+charLen*3.6;
 int xval = x; 
 display.setTextSize(size);
 display.setTextColor(SH110X_WHITE);
 
 
 display.setCursor(x, y);
 display.print(value,0);

 if(unit == "%")
 display.print(unit);

 else{  
   display.drawCircle(xo, y+2, 2, SH110X_WHITE);  // print degree symbols (  )
   display.setCursor(xunit, y);
   display.print(unit);
 }
 
}
