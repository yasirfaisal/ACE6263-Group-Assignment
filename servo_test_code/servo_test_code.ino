#include <Servo.h>

Servo gateServo;
Servo dispServo;

void setup() {
  gateServo.attach(21);    // ESP32 GPIO21
  dispServo.attach(17);    // ESP32 GPIO17

  // initialize to closed/rest
  gateServo.write(0);
  dispServo.write(0);
}

void dispenseMeal() {
  // Step 1: Open gate
  gateServo.write(90);
  delay(500);             // let the gate clear

  // Step 2: Push food
  dispServo.write(60);    // calibrated angle to deliver ~175 g
  delay(800);             // adjust for your mechanism speed

  // Step 3: Return paddle & close gate
  dispServo.write(0);
  delay(500);
  gateServo.write(0);
}
