#pragma once

#ifndef MICROCLAW_NODE_ID
#define MICROCLAW_NODE_ID "microclaw-esp32c3-001"
#endif

#ifndef MICROCLAW_WIFI_SSID
#define MICROCLAW_WIFI_SSID "CHANGE_ME"
#endif

#ifndef MICROCLAW_WIFI_PASSWORD
#define MICROCLAW_WIFI_PASSWORD "CHANGE_ME"
#endif

#ifndef MICROCLAW_MQTT_HOST
#define MICROCLAW_MQTT_HOST "192.168.1.10"
#endif

#ifndef MICROCLAW_MQTT_PORT
#define MICROCLAW_MQTT_PORT 1883
#endif

#ifndef MICROCLAW_DHT_PIN
#define MICROCLAW_DHT_PIN 4
#endif

#ifndef MICROCLAW_BATTERY_PIN
#define MICROCLAW_BATTERY_PIN 0
#endif

#ifndef MICROCLAW_LOW_BATTERY_MV
#define MICROCLAW_LOW_BATTERY_MV 3300
#endif

#ifndef MICROCLAW_SLEEP_SECONDS
#define MICROCLAW_SLEEP_SECONDS 300
#endif

#ifndef MICROCLAW_OTA_WINDOW_SECONDS
#define MICROCLAW_OTA_WINDOW_SECONDS 20
#endif

constexpr const char* kNodeId = MICROCLAW_NODE_ID;
constexpr const char* kWifiSsid = MICROCLAW_WIFI_SSID;
constexpr const char* kWifiPassword = MICROCLAW_WIFI_PASSWORD;
constexpr const char* kMqttHost = MICROCLAW_MQTT_HOST;
constexpr uint16_t kMqttPort = MICROCLAW_MQTT_PORT;
constexpr uint8_t kDhtPin = MICROCLAW_DHT_PIN;
constexpr uint8_t kBatteryPin = MICROCLAW_BATTERY_PIN;
constexpr uint16_t kLowBatteryMv = MICROCLAW_LOW_BATTERY_MV;
constexpr uint32_t kSleepSeconds = MICROCLAW_SLEEP_SECONDS;
constexpr uint32_t kOtaWindowSeconds = MICROCLAW_OTA_WINDOW_SECONDS;
