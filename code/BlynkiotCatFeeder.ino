#define BLYNK_TEMPLATE_ID "TMPL65PgjKywK"
#define BLYNK_TEMPLATE_NAME "Catfeeder"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>

// WiFi and Blynk credentials
char auth[] = "flI8X48AIyC6x0Gpn-xNNpxs_aRuN71Y";
char ssid[] = "Fawaz";
char pass[] = "fawaz2003";

// Pin configuration
const int SERVO1_PIN = 13;    // Gate
const int SERVO2_PIN = 12;    // Dispenser
const int TRIG_PIN = 23;      // Ultrasonic TRIG
const int ECHO_PIN = 22;      // Ultrasonic ECHO

#define VPIN_FEED_BUTTON V0

Servo servo1;
Servo servo2;
BlynkTimer timer;

unsigned long lastTriggerTime = 0;
const unsigned long cooldown = 10000;  // 10 seconds

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // Attach servos
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo1.write(0);
  servo2.write(0);

  // Ultrasonic setup
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Timer to check ultrasonic sensor every 500ms
  timer.setInterval(500L, checkUltrasonic);

  Serial.println("Cat Feeder with Servo + Ultrasonic + Blynk Ready");
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
    Serial.println("Cat detected nearby!");
    runServos();
    lastTriggerTime = millis();
  }
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // 30ms timeout
  return duration * 0.034 / 2.0;
}

void runServos() {
  Serial.println("Activating Servos...");

  servo2.write(0);    // Push food
  delay(700);

  servo2.write(90);   // Close dispenser

  servo1.write(360);  // Open gate
  delay(1000);

  servo1.write(0);    // Reset gate
  delay(700);

  Serial.println("Feeding cycle complete.");
}

void loop() {
  Blynk.run();
  timer.run();
}
