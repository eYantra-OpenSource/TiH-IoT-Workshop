/*
 * Exp1_Diwali_RGB.ino
 * 
 * Aim: With the use of RGB LED, blink it to visualize Diwali pattern.
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
  // put your setup code here, to run once:
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);

  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}

// turn ON RED LED
void led_redOn() {
  digitalWrite(RED_LED, LOW);
}

// turn OFF RED LED
void led_redOff() {
  digitalWrite(RED_LED, HIGH);
}

// turn ON BLUE LED
void led_blueOn() {
  digitalWrite(BLUE_LED, LOW);
}

// turn OFF BLUE LED
void led_blueOff() {
  digitalWrite(BLUE_LED, HIGH);
}

// turn ON GREEN LED
void led_greenOn() {
  digitalWrite(GREEN_LED, LOW);
}

// turn OFF GREEN LED
void led_greenOff() {
  digitalWrite(GREEN_LED, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // RED color
  led_redOn();
  delay(250);
  led_redOff();
  delay(250);

  // GREEN color
  led_greenOn();
  delay(250);
  led_greenOff();
  delay(250);

  // BLUE color
  led_blueOn();
  delay(250);
  led_blueOff();
  delay(250);

  // YELLOW color
  led_redOn();
  led_greenOn();
  delay(250);
  led_redOff();
  led_greenOff();
  delay(250);

  // CYAN color
  led_greenOn();
  led_blueOn();
  delay(250);
  led_greenOff();
  led_blueOff();
  delay(250);

  // MAGENTA color
  led_redOn();
  led_blueOn();
  delay(250);
  led_redOff();
  led_blueOff();
  delay(250);
}
