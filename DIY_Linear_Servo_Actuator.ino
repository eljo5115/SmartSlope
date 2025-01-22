
#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

void setup() {
  myservo.attach(10);  // attaches the servo on pin 10 to the servo object
  Serial.begin(9600);
}

void loop() {
    int analogValue = analogRead(A0);
    Serial.println(analogValue);

    if(analogValue > 200){ // if light is getting through ==== no ball, needs to get another one 
      Serial.println("No golf ball detected");
      myservo.write(22);  
      delay(1000);
      Serial.println("retriving ball");
      }
     if(analogValue < 10){
      Serial.println("ball on tee");
      delay(2000);
      myservo.write(155);  // moves backward (opposite direction)
    }
    }
    

