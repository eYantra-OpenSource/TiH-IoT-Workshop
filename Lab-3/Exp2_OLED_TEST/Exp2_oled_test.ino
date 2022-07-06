/*
 * Exp3_oled.ino
 * 
 * Aim: Display "Hello World!" on OLED Display. 
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 01-07-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * OLED SDA   -> SDA (PD1)  -> Arduino pin 20
 * OLED SCL   -> SCL (PD0)  -> Arduino pin 21
 */

// include Arduino SH1106 Adafruit library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// initialize oled display with the I2C addr - 0x3C
#define i2c_Address 0x3c 

// configure oled display with the width and height as 128x64 pixels
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()   {
  
  // enable the serial port
  Serial.begin(9600);

  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
 
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  // Display the text "Hello World!"
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.println("Hello World!\n");
  
  // Display text with inverted background
  display.setTextSize(1);
  display.setTextColor(SH110X_BLACK, SH110X_WHITE); // 'inverted' text
  display.print("TiH Workshop - ");display.println(2022);
  display.println();
  
  // Display the text "e-Yantra" with increased text size
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.println("e-Yantra");
  display.display();
  delay(2000);
  
}


void loop() {
  // invert the display
  display.invertDisplay(true);
  delay(2000);
  display.invertDisplay(false);
  delay(2000);
}
