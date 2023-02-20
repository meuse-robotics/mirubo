#include <ESP32Servo.h>

#define SV_PIN 13
Servo myServo;

void setup() {
  // put your setup code here, to run once:
  myServo.attach(SV_PIN);
  myServo.write(90);
}

void loop() {
  // put your main code here, to run repeatedly:
  myServo.write(120);
  delay(1000);
  myServo.write(60);
  delay(1000);
}
