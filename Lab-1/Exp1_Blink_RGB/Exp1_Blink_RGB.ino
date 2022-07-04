/*
 * Exp1_Blink_RGB.ino
 * 
 * Aim: With the use of RGB LED, blink one color at a time.
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 30-06-2022
 * Author:        e-Yantra Team
 * 
 * Connections:
 * Red LED    -> PH3 (OC4A) -> Arduino pin 6
 * Blue LED   -> PH4 (OC4B) -> Arduino pin 7
 * Green LED  -> PH5 (OC4C) -> Arduino pin 8
 * 
 * Note: All the LEDs are active low.
 */

// macro definitions
#define RED_LED   6
#define BLUE_LED  7
#define GREEN_LED 8

void setup() {
  // set RGB LED pins as Output
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  // turn OFF the LED initially
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}

void loop() {
  
  // BLUE color
  
  
  // RED color
  
  
  // GREEN color
  digitalWrite(GREEN_LED, LOW);
  delay(1000);
  digitalWrite(GREEN_LED, HIGH);
  delay(1000);
}
