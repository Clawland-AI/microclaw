
/**
 * MicroClaw – Sensor-level micro AI Agent
 * Runs on ESP32 / ESP32-C3 with <1MB RAM, $2-5 hardware cost.
 * Part of the Clawland edge AI agent network.
 *
 * Supported targets:
 *   - ESP32   (Xtensa LX6, dual-core)
 *   - ESP32-C3 (RISC-V, single-core, 802.11 b/g/n + BLE 5)
 */
#include <Arduino.h>
#include <WiFi.h>

const char* AGENT_NAME = "microclaw";
const char* VERSION = "0.1.0";

// ESP32-C3 uses USB Serial/JTAG peripheral for Serial0 by default.
// On USB-connected boards the hardware UART is mapped to Serial0 as well,
// but we explicitly call Serial.begin() so both variants work correctly.
void setup() {
    Serial.begin(115200);

    // Give the serial port a moment to enumerate (important on ESP32-C3 USB)
    delay(100);

    Serial.println("🐾 MicroClaw Agent v0.1.0");

#if defined(ARDUINO_ESP32C3_DEV) || defined(CONFIG_IDF_TARGET_ESP32C3)
    Serial.println("   Target : ESP32-C3 (RISC-V, single-core)");
#else
    Serial.println("   Target : ESP32 (Xtensa LX6)");
#endif

    Serial.println("   MCU-level sensor agent starting...");
    Serial.println("   Waiting for sensor configuration...");
}

void loop() {
    delay(1000);
}
