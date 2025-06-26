#define BLYNK_TEMPLATE_ID "TMPL65PgjKywK"
#define BLYNK_TEMPLATE_NAME "Catfeeder"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "HX711.h"

// Blynk credentials
char auth[] = "flI8X48AIyC6x0Gpn-xNNpxs_aRuN71Y";
char ssid[] = "Fawaz";
char pass[] = "fawaz2003";

// Pin configuration
const int SERVO1_PIN = 13;  // Gate
const int SERVO2_PIN = 12;  // Dispenser
const int TRIG_PIN = 23;
const int ECHO_PIN = 22;
const int HX711_DT = 19;
const int HX711_SCK = 18;

#define VPIN_FEED_BUTTON V0
#define VPIN_WEIGHT_DISPLAY V1

Servo servo1;
Servo servo2;
HX711 scale;
BlynkTimer timer;

unsigned long lastTriggerTime = 0;
const unsigned long cooldown = 10000; // 10s

// ✅ Your actual calibration factor for grams
float calibration_factor = 1790.0;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // Servo setup
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  // Ultrasonic setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // HX711 setup with actual load cell
  scale.begin(HX711_DT, HX711_SCK);
  delay(1000);
  scale.set_scale(calibration_factor);  // Apply your actual scale factor
  scale.tare();                         // Zero the scale

  Serial.println("Scale tared with calibration factor 1790.");

  // Set timers
  timer.setInterval(500L, checkUltrasonic);
  timer.setInterval(1000L, sendWeightToBlynk);

  Serial.println("SmartCatFeeder ready with real load sensor.");
}

BLYNK_WRITE(VPIN_FEED_BUTTON) {
  int state = param.asInt();
  if (state == 1) {
    runServos();
  }
}

void checkUltrasonic() {
  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance > 5 && distance < 30 && millis() - lastTriggerTime > cooldown) {
    Serial.println("Cat detected. Dispensing...");
    runServos();
    lastTriggerTime = millis();
  }
}

void sendWeightToBlynk() {
  if (scale.is_ready()) {
    float weight = scale.get_units(5);  // average of 5 readings
    Serial.print("Weight: ");
    Serial.print(weight, 2);
    Serial.println(" g");

    Blynk.virtualWrite(VPIN_WEIGHT_DISPLAY, weight);
  } else {
    Serial.println("HX711 not ready.");
  }
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // timeout 30ms
  return duration * 0.034 / 2.0;
}

void runServos() {
  Serial.println("Running feeding cycle...");

  servo2.write(0);  // Open gate
  delay(1100);
  servo2.write(90);    // Push food
  delay(1000);
  servo1.write(360);    // Reset gate
  delay(1000);
  servo2.write(0);   // Close dispenser
  delay(1000);
  Serial.println("Feeding complete.");
}

void loop() {
  Blynk.run();
  timer.run();
}
