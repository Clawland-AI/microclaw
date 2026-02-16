/**
 * MicroClaw — Sensor-level micro AI Agent
 * Runs on ESP32 with <1MB RAM, $2-5 hardware cost.
 * Part of the Clawland edge AI agent network.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// ========== Configuration ==========
const char* AGENT_NAME = "microclaw";
const char* VERSION = "0.2.0";

// WiFi Configuration (update with your credentials)
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// MQTT Configuration
const char* MQTT_BROKER = "mqtt.clawland.local";  // Configurable broker URL
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "microclaw_node_001";  // Unique node ID
const char* NODE_ID = "node_001";  // Used in topic path

// Sensor Configuration
#define DHT_PIN 4          // GPIO pin for DHT22
#define DHT_TYPE DHT22
#define PUBLISH_INTERVAL 60000  // 60 seconds (configurable)

// Topic format: clawland/{node_id}/sensors/{sensor_type}
const char* TOPIC_TEMPERATURE = "clawland/node_001/sensors/temperature";
const char* TOPIC_HUMIDITY = "clawland/node_001/sensors/humidity";
const char* TOPIC_STATUS = "clawland/node_001/status";

// ========== Global Objects ==========
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastPublish = 0;

// ========== Function Declarations ==========
void connectWiFi();
void connectMQTT();
void reconnectMQTT();
void publishSensorData();

// ========== Setup ==========
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n🍇 MicroClaw Agent v" + String(VERSION));
    Serial.println("   MCU-level sensor agent starting...");
    Serial.println("   MQTT Client enabled");
    
    // Initialize DHT sensor
    dht.begin();
    Serial.println("   ✓ DHT22 sensor initialized");
    
    // Connect to WiFi
    connectWiFi();
    
    // Configure MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setKeepAlive(60);
    mqttClient.setBufferSize(512);
    
    // Initial MQTT connection
    connectMQTT();
    
    Serial.println("   ✓ MicroClaw ready");
}

// ========== Main Loop ==========
void loop() {
    // Auto-reconnect MQTT if disconnected
    if (!mqttClient.connected()) {
        reconnectMQTT();
    }
    mqttClient.loop();
    
    // Publish sensor data at configured interval
    unsigned long now = millis();
    if (now - lastPublish >= PUBLISH_INTERVAL) {
        publishSensorData();
        lastPublish = now;
    }
}

// ========== WiFi Connection ==========
void connectWiFi() {
    Serial.print("   Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n   ✓ WiFi connected");
        Serial.print("   IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\n   ✗ WiFi connection failed");
    }
}

// ========== MQTT Initial Connection ==========
void connectMQTT() {
    Serial.print("   Connecting to MQTT broker: ");
    Serial.println(MQTT_BROKER);
    
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
        Serial.println("   ✓ MQTT connected");
        
        // Publish online status
        mqttClient.publish(TOPIC_STATUS, "{\"status\":\"online\",\"version\":\"0.2.0\"}", true);
    } else {
        Serial.print("   ✗ MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
    }
}

// ========== MQTT Auto-Reconnect ==========
void reconnectMQTT() {
    static unsigned long lastAttempt = 0;
    unsigned long now = millis();
    
    // Try reconnect every 5 seconds
    if (now - lastAttempt < 5000) {
        return;
    }
    lastAttempt = now;
    
    Serial.print("   Attempting MQTT reconnection...");
    
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
        Serial.println(" ✓");
        mqttClient.publish(TOPIC_STATUS, "{\"status\":\"reconnected\"}", true);
    } else {
        Serial.print(" ✗ (rc=");
        Serial.print(mqttClient.state());
        Serial.println(")");
    }
}

// ========== Publish Sensor Data ==========
void publishSensorData() {
    // Read DHT22 sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    // Check if readings are valid
    if (isnan(temperature) || isnan(humidity)) {
        Serial.println("   ✗ Failed to read DHT sensor");
        return;
    }
    
    // Publish temperature
    char tempPayload[50];
    snprintf(tempPayload, sizeof(tempPayload), "{\"value\":%.2f,\"unit\":\"C\"}", temperature);
    
    if (mqttClient.publish(TOPIC_TEMPERATURE, tempPayload)) {
        Serial.print("   📊 Published temp: ");
        Serial.print(temperature);
        Serial.println("°C");
    } else {
        Serial.println("   ✗ Failed to publish temperature");
    }
    
    // Publish humidity
    char humPayload[50];
    snprintf(humPayload, sizeof(humPayload), "{\"value\":%.2f,\"unit\":\"%%\"}", humidity);
    
    if (mqttClient.publish(TOPIC_HUMIDITY, humPayload)) {
        Serial.print("   📊 Published humidity: ");
        Serial.print(humidity);
        Serial.println("%");
    } else {
        Serial.println("   ✗ Failed to publish humidity");
    }
}
