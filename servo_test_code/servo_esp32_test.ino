#include <ESP32Servo.h>  // Install via Library Manager if you haven’t already

Servo myServo;
const int SERVO_PIN = 13;  // D13 on your ESP32 dev board

void setup() {
  myServo.attach(SERVO_PIN);    // attach servo to pin D13
}

void loop() {
  // sweep from 0° to 180°
  for (int pos = 0; pos <= 180; pos += 5) {
    myServo.write(pos);
    delay(50);
  }
  // sweep back from 180° to 0°
  for (int pos = 180; pos >= 0; pos -= 5) {
    myServo.write(pos);
    delay(50);
  }
}
