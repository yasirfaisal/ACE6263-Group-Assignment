#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <HX711.h>

// Replace with your Blynk auth token and Wi-Fi credentials
char auth[]   = "YourAuthToken";
char ssid[]   = "YourSSID";
char pass[]   = "YourPassword";

// Pin assignments
const int SERVO_GATE_PIN  = 21;  // Gate servo
const int SERVO_DISP_PIN  = 17;  // Dispense servo
const int HX711_DT        = 19;  // HX711 data
const int HX711_SCK       = 18;  // HX711 clock
const int TRIG_PIN        = 23;  // HC-SR04 trigger
const int ECHO_PIN        = 22;  // HC-SR04 echo (via divider)

// Blynk virtual pins
#define VPIN_WEIGHT    V1
#define VPIN_PROXIMITY V2
#define VPIN_FEED_BTN  V3

Servo gateServo;
Servo dispServo;
HX711 scale;
BlynkTimer timer;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);

  // Attach servos
  gateServo.attach(SERVO_GATE_PIN, 500, 2400);  // pulse limits in µs
  dispServo.attach(SERVO_DISP_PIN, 500, 2400);
  gateServo.write(0);
  dispServo.write(0);

  // Initialize HX711
  scale.begin(HX711_DT, HX711_SCK);
  scale.set_scale(2280.f);  // calibration factor (adjust in code)
  scale.tare();

  // Initialize HC-SR04 pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Send sensor data every 30 seconds
  timer.setInterval(30000L, sendSensorData);
}

// Main loop
void loop() {
  Blynk.run();
  timer.run();
}

// Blynk button press handler to trigger feeding
BLYNK_WRITE(VPIN_FEED_BTN) {
  if (param.asInt()) {
    dispenseMeal();
    // Reset the button in app
    Blynk.virtualWrite(VPIN_FEED_BTN, 0);
  }
}

// Function to perform a single feed cycle
void dispenseMeal() {
  // 1. Open gate
  gateServo.write(90);
  delay(500);

  // 2. Rotate dispense servo to push food
  dispServo.write(60);
  delay(800);
  dispServo.write(0);
  delay(500);

  // 3. Close gate
  gateServo.write(0);

  // 4. Update readings on Blynk
  sendSensorData();
}

// Read sensors and send to Blynk
void sendSensorData() {
  // Read weight (grams)
  float weight = scale.get_units(10);
  Blynk.virtualWrite(VPIN_WEIGHT, weight);

  // Read distance (cm)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;
  Blynk.virtualWrite(VPIN_PROXIMITY, distance);
}