/**
 * MicroClaw - sensor-level edge agent for ESP32-class MCUs.
 *
 * This firmware keeps runtime configuration compile-time driven so example
 * builds never commit Wi-Fi, MQTT, or OTA credentials.
 */
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <cstring>

#ifndef MICROCLAW_BOARD_NAME
#define MICROCLAW_BOARD_NAME "esp32dev"
#endif

#ifndef MICROCLAW_NODE_ID
#define MICROCLAW_NODE_ID "microclaw-001"
#endif

#ifndef MICROCLAW_WIFI_SSID
#define MICROCLAW_WIFI_SSID ""
#endif

#ifndef MICROCLAW_WIFI_PASSWORD
#define MICROCLAW_WIFI_PASSWORD ""
#endif

#ifndef MICROCLAW_MQTT_HOST
#define MICROCLAW_MQTT_HOST ""
#endif

#ifndef MICROCLAW_MQTT_PORT
#define MICROCLAW_MQTT_PORT 1883
#endif

#ifndef MICROCLAW_MQTT_TOPIC
#define MICROCLAW_MQTT_TOPIC "clawland/microclaw/status"
#endif

#ifndef MICROCLAW_ANALOG_PIN
#define MICROCLAW_ANALOG_PIN 34
#endif

#ifndef MICROCLAW_DHT_PIN
#define MICROCLAW_DHT_PIN 4
#endif

#ifndef MICROCLAW_DHT_TYPE
#define MICROCLAW_DHT_TYPE DHT22
#endif

#ifndef MICROCLAW_REPORT_INTERVAL_MS
#define MICROCLAW_REPORT_INTERVAL_MS 10000UL
#endif

#ifndef MICROCLAW_MQTT_RETRY_MS
#define MICROCLAW_MQTT_RETRY_MS 5000UL
#endif

#ifndef MICROCLAW_ENABLE_OTA
#define MICROCLAW_ENABLE_OTA 1
#endif

const char* AGENT_NAME = "microclaw";
const char* VERSION = "0.2.0";

DHT dht(MICROCLAW_DHT_PIN, MICROCLAW_DHT_TYPE);
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

unsigned long lastReportAt = 0;
unsigned long lastMqttAttemptAt = 0;

struct SensorSnapshot {
    int analogRaw;
    int analogMillivolts;
    float temperatureC;
    float humidityPct;
    bool dhtOk;
};

static bool hasText(const char* value) {
    return value != nullptr && value[0] != '\0';
}

static void connectWifi() {
    if (!hasText(MICROCLAW_WIFI_SSID) || WiFi.status() == WL_CONNECTED) {
        return;
    }

    Serial.print("Connecting Wi-Fi SSID ");
    Serial.println(MICROCLAW_WIFI_SSID);
    WiFi.mode(WIFI_STA);
    WiFi.begin(MICROCLAW_WIFI_SSID, MICROCLAW_WIFI_PASSWORD);

    const unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000UL) {
        delay(250);
        Serial.print('.');
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Wi-Fi connected: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Wi-Fi unavailable; continuing offline.");
    }
}

static void setupOta() {
#if MICROCLAW_ENABLE_OTA
    if (WiFi.status() != WL_CONNECTED) {
        return;
    }

    ArduinoOTA.setHostname(MICROCLAW_NODE_ID);
    ArduinoOTA.onStart([]() {
        Serial.println("OTA update starting");
    });
    ArduinoOTA.onEnd([]() {
        Serial.println("OTA update complete");
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.print("OTA error: ");
        Serial.println(static_cast<int>(error));
    });
    ArduinoOTA.begin();
    Serial.println("OTA ready");
#endif
}

static void setupMqtt() {
    if (!hasText(MICROCLAW_MQTT_HOST)) {
        Serial.println("MQTT host not configured; status reporting disabled.");
        return;
    }

    mqtt.setServer(MICROCLAW_MQTT_HOST, MICROCLAW_MQTT_PORT);
    mqtt.setBufferSize(384);
}

static void connectMqtt() {
    if (!hasText(MICROCLAW_MQTT_HOST) || WiFi.status() != WL_CONNECTED || mqtt.connected()) {
        return;
    }

    const unsigned long now = millis();
    if (now - lastMqttAttemptAt < MICROCLAW_MQTT_RETRY_MS) {
        return;
    }
    lastMqttAttemptAt = now;

    Serial.print("Connecting MQTT ");
    Serial.print(MICROCLAW_MQTT_HOST);
    Serial.print(':');
    Serial.println(MICROCLAW_MQTT_PORT);

    if (mqtt.connect(MICROCLAW_NODE_ID)) {
        Serial.println("MQTT connected");
    } else {
        Serial.print("MQTT connect failed rc=");
        Serial.println(mqtt.state());
    }
}

static SensorSnapshot readSensors() {
    SensorSnapshot snapshot;
    snapshot.analogRaw = analogRead(MICROCLAW_ANALOG_PIN);
    snapshot.analogMillivolts = analogReadMilliVolts(MICROCLAW_ANALOG_PIN);
    snapshot.temperatureC = dht.readTemperature();
    snapshot.humidityPct = dht.readHumidity();
    snapshot.dhtOk = !isnan(snapshot.temperatureC) && !isnan(snapshot.humidityPct);
    return snapshot;
}

static void publishSnapshot(const SensorSnapshot& snapshot) {
    char payload[320];
    char temperature[16];
    char humidity[16];

    if (snapshot.dhtOk) {
        dtostrf(snapshot.temperatureC, 0, 2, temperature);
        dtostrf(snapshot.humidityPct, 0, 2, humidity);
    } else {
        strcpy(temperature, "null");
        strcpy(humidity, "null");
    }

    snprintf(
        payload,
        sizeof(payload),
        "{\"nodeId\":\"%s\",\"agent\":\"%s\",\"version\":\"%s\",\"board\":\"%s\",\"uptimeMs\":%lu,\"analogRaw\":%d,\"analogMv\":%d,\"temperatureC\":%s,\"humidityPct\":%s,\"wifiRssi\":%d}",
        MICROCLAW_NODE_ID,
        AGENT_NAME,
        VERSION,
        MICROCLAW_BOARD_NAME,
        millis(),
        snapshot.analogRaw,
        snapshot.analogMillivolts,
        temperature,
        humidity,
        WiFi.status() == WL_CONNECTED ? WiFi.RSSI() : 0
    );

    Serial.println(payload);
    if (mqtt.connected()) {
        mqtt.publish(MICROCLAW_MQTT_TOPIC, payload, false);
    }
}

void setup() {
    Serial.begin(115200);
    delay(100);

    Serial.println("MicroClaw Agent v0.2.0");
    Serial.print("Board: ");
    Serial.println(MICROCLAW_BOARD_NAME);
    Serial.print("Node: ");
    Serial.println(MICROCLAW_NODE_ID);

    pinMode(MICROCLAW_ANALOG_PIN, INPUT);
    analogReadResolution(12);
    dht.begin();

    connectWifi();
    setupOta();
    setupMqtt();
}

void loop() {
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }

#if MICROCLAW_ENABLE_OTA
    if (WiFi.status() == WL_CONNECTED) {
        ArduinoOTA.handle();
    }
#endif

    connectMqtt();
    mqtt.loop();

    const unsigned long now = millis();
    if (now - lastReportAt >= MICROCLAW_REPORT_INTERVAL_MS) {
        lastReportAt = now;
        publishSnapshot(readSensors());
    }

    delay(10);
}
