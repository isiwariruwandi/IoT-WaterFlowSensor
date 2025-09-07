#include <ESP8266WiFi.h>
// === Motor (Relay) Configuration ===
#define RELAY_PIN D1  // GPIO5 on NodeMCU

// === Flow Sensor Configuration ===
const byte flowSensorPin = D2;       // GPIO4 (D2 on NodeMCU)
volatile int pulseCount = 0;
const float calibrationFactor = 7.5; // Sensor-specific
float flowRate;     // Liters per minute
float volume;       // Total volume in liters
unsigned long lastFlowCalcTime = 0;

// === Flow Sensor Interrupt Handler ===
void IRAM_ATTR pulseCounter() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);

  // Setup Relay (Motor) — turn ON permanently
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // Turn motor ON
  Serial.println("Motor turned ON permanently.");

  // Setup Flow Sensor
  pinMode(flowSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);
  lastFlowCalcTime = millis();
  volume = 0;
}

void loop() {
  unsigned long currentMillis = millis();

  // === Flow Sensor Reading Every 1 Second ===
  if (currentMillis - lastFlowCalcTime >= 1000) {
    detachInterrupt(digitalPinToInterrupt(flowSensorPin));  // Temporarily disable interrupts

    flowRate = pulseCount / calibrationFactor;              // Calculate flow rate
    float litersPerSecond = flowRate / 60.0;
    volume += litersPerSecond;

    Serial.print("Flow Rate: ");
    Serial.print(flowRate);
    Serial.print(" L/min\t");

    Serial.print("Total Volume: ");
    Serial.print(volume, 3);
    Serial.println(" L");

    pulseCount = 0;
    lastFlowCalcTime = currentMillis;

    attachInterrupt(digitalPinToInterrupt(flowSensorPin), pulseCounter, RISING);  // Re-enable interrupts
  }
}