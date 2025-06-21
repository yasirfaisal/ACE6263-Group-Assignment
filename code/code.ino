#define BLYNK_TEMPLATE_ID "TMPL65PgjKywK"
#define BLYNK_TEMPLATE_NAME "Catfeeder"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "HX711.h"

// Blynk credentials
char auth[] = "YourAuthTokenHere";
char ssid[] = "YourWiFiSSID";
char pass[] = "YourWiFiPassword";

// Pins
const int GATE_SERVO_PIN = 13;
const int DISPENSER_SERVO_PIN = 12;
const int HX711_DT = 19;
const int HX711_SCK = 18;
const int TRIG_PIN = 25;
const int ECHO_PIN = 26;

// Blynk virtual button pin
#define VPIN_FEED_BUTTON V0

Servo gateServo;
Servo dispenserServo;
HX711 scale;

unsigned long lastDispenseTime = 0;
const unsigned long cooldown = 10000; // 10 sec cooldown

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Blynk and Wi-Fi
  Blynk.begin(auth, ssid, pass);

  // Servo setup
  gateServo.attach(GATE_SERVO_PIN);
  dispenserServo.attach(DISPENSER_SERVO_PIN);
  gateServo.write(0);
  dispenserServo.write(0);

  // HX711
  scale.begin(HX711_DT, HX711_SCK);
  delay(2000);
  scale.set_scale();
  scale.tare();

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("SmartCatFeeder Ready (Blynk + Ultrasonic)");
}

// Blynk button: manual feed trigger
BLYNK_WRITE(VPIN_FEED_BUTTON) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    dispenseFood();
  }
}

void loop() {
  Blynk.run();

  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Auto-detect via ultrasonic
  if (distance > 5 && distance < 30 && millis() - lastDispenseTime > cooldown) {
    Serial.println("Cat detected via ultrasonic. Dispensing...");
    dispenseFood();
  }

  delay(300);
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  return duration * 0.034 / 2.0;
}

void dispenseFood() {
  lastDispenseTime = millis();

  gateServo.write(90);
  delay(500);

  dispenserServo.write(60);
  delay(700);
  dispenserServo.write(0);
  delay(500);

  gateServo.write(0);
  Serial.println("Food dispensed.");
}
