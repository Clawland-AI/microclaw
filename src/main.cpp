/**
 * MicroClaw 鈥?Sensor-level micro AI Agent
 * Runs on ESP32 with <1MB RAM, $2-5 hardware cost.
 * Part of the Clawland edge AI agent network.
 */
#include <Arduino.h>
#include <WiFi.h>

const char* AGENT_NAME = "microclaw";
const char* VERSION = "0.1.0";

void setup() {
    Serial.begin(115200);
    Serial.println("馃 MicroClaw Agent v0.1.0");
    Serial.println("   MCU-level sensor agent starting...");
    Serial.println("   Waiting for sensor configuration...");
}

void loop() {
    delay(1000);
}