*
*/
* Exp3_PWM.ino
 * 
 * Aim: To control the intensity of bulb using PWM pulses
 * 
 * Target:        eYFi-Mega dev board (https://products.e-yantra.org/eyfi-mega/)
 * Last modified: 03-07-2022
 * Author:        e-Yantra Team
 * 
 * Connections of bulb:
 * Terminal 1  -> PB4 (OC2A) -> Arduino pin 10
 * Terminal 2  -> any GND pin
 * 
 */


// macro definitions
#define BULB_1 45
#define FAN_1 46
String incomingString;

void setup() {
  Serial.begin(9600);
  // Setting the required pin as output
  pinMode(BULB_1,OUTPUT);
  analogWrite(BULB_1,0);
  pinMode(FAN_1,OUTPUT);
  analogWrite(FAN_1,0);
}

void loop() {

  // Serial.print("FAN on");

  if (Serial.available() > 0) 
  {
    // read the incoming byte:
    incomingString = Serial.readString();
    incomingString.trim();
   // say what you got:
    Serial.print("I received: ");
    Serial.println(incomingString);

    if(incomingString=="LED warm")
    {
      analogWrite(BULB_1,255);
      Serial.print("WARM LED has turned on !! ");
    }
    if(incomingString=="LED cool")
    {
      analogWrite(BULB_1,128);  
      Serial.print("COOL LED has turned on !! ");   
    }
    if(incomingString=="LED off")
    {
      analogWrite(BULB_1,0);
      Serial.print("LED has turned OFF !! ");
    }
    if(incomingString=="FAN on")
    {
      analogWrite(FAN_1,255);  
      Serial.print("Fan has turned on !! ");
    }
    if(incomingString=="FAN off")
    {
      analogWrite(FAN_1,0);
      Serial.print("Fan has turned off !! ");
    }
  }
}
