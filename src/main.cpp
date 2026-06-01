/**
 * MicroClaw ESP32-C3 port.
 *
 * Reads local sensors, reports upstream through MQTT, exposes a short OTA
 * window, and returns to deep sleep for battery-powered deployments.
 */
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <esp_sleep.h>

#include "BatteryMonitor.h"
#include "DHT22Sensor.h"
#include "MicroClawConfig.h"
#include "MicroClawMqtt.h"

const char* kVersion = "0.2.0";
constexpr float kBatteryDividerRatio = 2.0f;

DHT22Sensor dht22(kDhtPin);
BatteryMonitor battery(kBatteryPin, kBatteryDividerRatio);
MicroClawMqtt mqtt(kNodeId, kMqttHost, kMqttPort);

static bool connectWiFi(uint32_t timeoutMs) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(kWifiSsid, kWifiPassword);

  const uint32_t deadline = millis() + timeoutMs;
  while (WiFi.status() != WL_CONNECTED && millis() < deadline) {
    delay(250);
  }
  return WiFi.status() == WL_CONNECTED;
}

static String sensorJson(const DHT22Reading& reading, uint16_t batteryMv) {
  String json = "{";
  json += "\"node_id\":\"" + String(kNodeId) + "\",";
  json += "\"sensor_type\":\"dht22\",";
  json += "\"temperature_c\":" + String(reading.temperatureC, 1) + ",";
  json += "\"humidity_percent\":" + String(reading.humidityPercent, 1) + ",";
  json += "\"battery_mv\":" + String(batteryMv) + ",";
  json += "\"firmware\":\"" + String(kVersion) + "\"";
  json += "}";
  return json;
}

static String statusJson(const char* state, uint16_t batteryMv) {
  String json = "{";
  json += "\"node_id\":\"" + String(kNodeId) + "\",";
  json += "\"state\":\"" + String(state) + "\",";
  json += "\"battery_mv\":" + String(batteryMv) + ",";
  json += "\"sleep_seconds\":" + String(kSleepSeconds) + ",";
  json += "\"ota_window_seconds\":" + String(kOtaWindowSeconds) + ",";
  json += "\"firmware\":\"" + String(kVersion) + "\"";
  json += "}";
  return json;
}

static String errorJson(const char* code, uint16_t batteryMv) {
  String json = "{";
  json += "\"node_id\":\"" + String(kNodeId) + "\",";
  json += "\"code\":\"" + String(code) + "\",";
  json += "\"battery_mv\":" + String(batteryMv);
  json += "}";
  return json;
}

static void configureOTA() {
  ArduinoOTA.setHostname(kNodeId);
  ArduinoOTA.begin();
}

static void runOTAWindow() {
  const uint32_t deadline = millis() + (kOtaWindowSeconds * 1000UL);
  while (millis() < deadline) {
    ArduinoOTA.handle();
    mqtt.loop();
    delay(20);
  }
}

static void sleepUntilNextCycle() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(kSleepSeconds) * 1000000ULL);
  Serial.printf("Sleeping for %lu seconds\n", static_cast<unsigned long>(kSleepSeconds));
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.printf("MicroClaw Agent %s starting as %s\n", kVersion, kNodeId);

  dht22.begin();
  battery.begin();

  const uint16_t batteryMv = battery.readMillivolts();
  const DHT22Reading reading = dht22.read();

  if (!connectWiFi(12000)) {
    Serial.println("WiFi unavailable; returning to deep sleep");
    sleepUntilNextCycle();
  }

  mqtt.begin();
  configureOTA();

  mqtt.publishStatus(statusJson("awake", batteryMv));

  if (reading.ok) {
    mqtt.publishSensor("dht22", sensorJson(reading, batteryMv));
  } else {
    mqtt.publishAlert("warning", errorJson(reading.error, batteryMv));
  }

  if (battery.isLow(batteryMv, kLowBatteryMv)) {
    mqtt.publishAlert("critical", errorJson("battery_low", batteryMv));
  }

  runOTAWindow();
  mqtt.publishStatus(statusJson("sleeping", batteryMv));
  sleepUntilNextCycle();
}

void loop() {
  ArduinoOTA.handle();
  mqtt.loop();
}
