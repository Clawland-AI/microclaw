/**
 * MicroClaw - ESP32 sensor-level micro agent.
 *
 * Reads a DHT22 sensor, publishes telemetry to MQTT, accepts simple fleet
 * commands, and optionally enters deep sleep between samples for battery nodes.
 */
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <esp_sleep.h>

#ifndef MICROCLAW_WIFI_SSID
#define MICROCLAW_WIFI_SSID ""
#endif

#ifndef MICROCLAW_WIFI_PASSWORD
#define MICROCLAW_WIFI_PASSWORD ""
#endif

#ifndef MICROCLAW_MQTT_HOST
#define MICROCLAW_MQTT_HOST "192.168.1.10"
#endif

#ifndef MICROCLAW_MQTT_PORT
#define MICROCLAW_MQTT_PORT 1883
#endif

#ifndef MICROCLAW_NODE_ID
#define MICROCLAW_NODE_ID "microclaw-esp32"
#endif

#ifndef MICROCLAW_DHT_PIN
#define MICROCLAW_DHT_PIN 4
#endif

#ifndef MICROCLAW_WAKE_INTERVAL_SECONDS
#define MICROCLAW_WAKE_INTERVAL_SECONDS 60
#endif

#ifndef MICROCLAW_DEEP_SLEEP_ENABLED
#define MICROCLAW_DEEP_SLEEP_ENABLED 0
#endif

const char *AGENT_NAME = "microclaw";
const char *VERSION = "0.1.0";
const char *WIFI_SSID = MICROCLAW_WIFI_SSID;
const char *WIFI_PASSWORD = MICROCLAW_WIFI_PASSWORD;
const char *MQTT_HOST = MICROCLAW_MQTT_HOST;
const int MQTT_PORT = MICROCLAW_MQTT_PORT;
const char *NODE_ID = MICROCLAW_NODE_ID;
const uint8_t DHT_PIN = MICROCLAW_DHT_PIN;
const uint8_t DHT_TYPE = DHT22;
const uint32_t WAKE_INTERVAL_SECONDS = MICROCLAW_WAKE_INTERVAL_SECONDS;
const bool DEEP_SLEEP_ENABLED = MICROCLAW_DEEP_SLEEP_ENABLED;

const uint64_t uS_TO_S_FACTOR = 1000000ULL;
const uint32_t SENSOR_READ_INTERVAL_MS = 30000;

float TEMP_ALERT_C = 35.0;
float HUMIDITY_ALERT_PCT = 70.0;

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastTelemetryMs = 0;

String topic(const char *suffix) {
    return String("microclaw/") + NODE_ID + suffix;
}

void publishStatus(const char *status, const char *detail = nullptr) {
    if (!mqtt.connected()) {
        return;
    }

    JsonDocument doc;
    doc["agent"] = AGENT_NAME;
    doc["version"] = VERSION;
    doc["node_id"] = NODE_ID;
    doc["status"] = status;
    doc["uptime_ms"] = millis();
    doc["heap_free"] = ESP.getFreeHeap();
    if (detail != nullptr) {
        doc["detail"] = detail;
    }

    char payload[256];
    size_t written = serializeJson(doc, payload, sizeof(payload));
    mqtt.publish(topic("/status").c_str(), reinterpret_cast<const uint8_t *>(payload), written, true);
}

void goToDeepSleep() {
    publishStatus("sleeping", "timer wake configured");
    mqtt.loop();
    delay(100);
    esp_sleep_enable_timer_wakeup(static_cast<uint64_t>(WAKE_INTERVAL_SECONDS) * uS_TO_S_FACTOR);
    Serial.printf("Entering deep sleep for %lu seconds\n", static_cast<unsigned long>(WAKE_INTERVAL_SECONDS));
    esp_deep_sleep_start();
}

bool connectWiFi() {
    if (strlen(WIFI_SSID) == 0) {
        Serial.println("WiFi SSID is empty; set MICROCLAW_WIFI_SSID in platformio build_flags");
        return false;
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.printf("Connecting WiFi SSID=%s", WIFI_SSID);

    unsigned long startedAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startedAt < 15000) {
        Serial.print('.');
        delay(500);
    }
    Serial.println();

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi connection failed");
        return false;
    }

    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());
    return true;
}

void publishTelemetry() {
    float temperature_c = dht.readTemperature();
    float humidity_pct = dht.readHumidity();

    if (isnan(temperature_c) || isnan(humidity_pct)) {
        publishStatus("sensor_error", "DHT22 read failed");
        return;
    }

    bool alert = temperature_c >= TEMP_ALERT_C || humidity_pct >= HUMIDITY_ALERT_PCT;

    JsonDocument doc;
    doc["agent"] = AGENT_NAME;
    doc["version"] = VERSION;
    doc["node_id"] = NODE_ID;
    doc["temperature_c"] = temperature_c;
    doc["humidity_pct"] = humidity_pct;
    doc["temp_alert_c"] = TEMP_ALERT_C;
    doc["humidity_alert_pct"] = HUMIDITY_ALERT_PCT;
    doc["alert"] = alert;
    doc["uptime_ms"] = millis();
    doc["heap_free"] = ESP.getFreeHeap();
    doc["wake_interval_seconds"] = WAKE_INTERVAL_SECONDS;

    char payload[384];
    size_t written = serializeJson(doc, payload, sizeof(payload));
    mqtt.publish(topic("/telemetry").c_str(), reinterpret_cast<const uint8_t *>(payload), written, true);

    if (alert) {
        mqtt.publish(topic("/alert").c_str(), reinterpret_cast<const uint8_t *>(payload), written, true);
    }

    Serial.printf("DHT22 temperature=%.1fC humidity=%.1f%% alert=%s\n",
                  temperature_c,
                  humidity_pct,
                  alert ? "true" : "false");
}

void handleCommand(char *commandTopic, byte *payload, unsigned int length) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload, length);
    if (err) {
        publishStatus("command_error", "invalid JSON");
        return;
    }

    const char *type = doc["type"] | "sample";
    Serial.printf("Received command on %s: %s\n", commandTopic, type);

    if (strcmp(type, "sample") == 0) {
        publishTelemetry();
        return;
    }

    if (strcmp(type, "sleep") == 0) {
        goToDeepSleep();
        return;
    }

    if (strcmp(type, "set_thresholds") == 0) {
        TEMP_ALERT_C = doc["temp_alert_c"] | TEMP_ALERT_C;
        HUMIDITY_ALERT_PCT = doc["humidity_alert_pct"] | HUMIDITY_ALERT_PCT;
        publishStatus("thresholds_updated");
        return;
    }

    publishStatus("command_ignored", type);
}

bool connectMqtt() {
    mqtt.setServer(MQTT_HOST, MQTT_PORT);
    mqtt.setCallback(handleCommand);

    unsigned long startedAt = millis();
    while (!mqtt.connected() && millis() - startedAt < 10000) {
        Serial.printf("Connecting MQTT %s:%d as %s\n", MQTT_HOST, MQTT_PORT, NODE_ID);
        if (mqtt.connect(NODE_ID, topic("/status").c_str(), 1, true, "offline")) {
            mqtt.publish(topic("/status").c_str(), "online", true);
            mqtt.subscribe(topic("/command").c_str(), 1);
            publishStatus("online");
            return true;
        }
        delay(1000);
    }

    Serial.printf("MQTT connection failed, state=%d\n", mqtt.state());
    return false;
}

void setup() {
    Serial.begin(115200);
    delay(100);
    Serial.printf("MicroClaw Agent %s (%s) starting\n", VERSION, NODE_ID);

    dht.begin();

    bool wifiReady = connectWiFi();
    bool mqttReady = wifiReady && connectMqtt();
    if (mqttReady) {
        publishTelemetry();
    }

    if (DEEP_SLEEP_ENABLED) {
        goToDeepSleep();
    }
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWiFi();
    }
    if (!mqtt.connected()) {
        connectMqtt();
    }
    mqtt.loop();

    if (millis() - lastTelemetryMs >= SENSOR_READ_INTERVAL_MS) {
        lastTelemetryMs = millis();
        publishTelemetry();
        if (DEEP_SLEEP_ENABLED) {
            goToDeepSleep();
        }
    }

    delay(100);
}
