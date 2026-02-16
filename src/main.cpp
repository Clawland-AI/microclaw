/**
 * MicroClaw — Sensor-level micro AI Agent
 * Runs on ESP32 with <1MB RAM, $2-5 hardware cost.
 * Part of the Clawland edge AI agent network.
 * 
 * Features:
 * - WiFi connectivity
 * - MQTT client for upstream reporting
 * - Sensor data aggregation
 * - Low-power operation
 */
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configuration (set via environment or hardcode for testing)
const char* AGENT_NAME = "microclaw";
const char* VERSION = "0.1.0";

// WiFi credentials (replace with your network)
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";

// MQTT broker configuration (replace with your broker)
const char* MQTT_BROKER = "mqtt.example.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "microclaw-esp32";
const char* MQTT_USERNAME = "";  // Optional
const char* MQTT_PASSWORD = "";  // Optional

// MQTT topics
const char* MQTT_TOPIC_STATUS = "microclaw/status";
const char* MQTT_TOPIC_SENSORS = "microclaw/sensors";
const char* MQTT_TOPIC_COMMANDS = "microclaw/commands";

// Globals
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
unsigned long lastReconnectAttempt = 0;
unsigned long lastSensorPublish = 0;
const unsigned long SENSOR_PUBLISH_INTERVAL = 30000;  // 30 seconds

// Function declarations
void setupWiFi();
void connectMQTT();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void publishStatus();
void publishSensorData();

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("========================================");
    Serial.println("🐾 MicroClaw Agent v" + String(VERSION));
    Serial.println("   MCU-level sensor agent starting...");
    Serial.println("========================================");
    
    // Setup WiFi
    setupWiFi();
    
    // Setup MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    mqttClient.setCallback(mqttCallback);
    
    // Initial connection
    connectMQTT();
    
    // Publish startup status
    publishStatus();
    
    Serial.println("✅ MicroClaw initialized and ready!");
}

void loop() {
    // Maintain MQTT connection
    if (!mqttClient.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            Serial.println("⚠️  MQTT disconnected, reconnecting...");
            connectMQTT();
        }
    } else {
        mqttClient.loop();
    }
    
    // Publish sensor data periodically
    unsigned long now = millis();
    if (now - lastSensorPublish > SENSOR_PUBLISH_INTERVAL) {
        lastSensorPublish = now;
        publishSensorData();
    }
    
    delay(10);  // Small delay to prevent watchdog reset
}

void setupWiFi() {
    Serial.print("📡 Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("✅ WiFi connected!");
        Serial.print("   IP address: ");
        Serial.println(WiFi.localIP());
        Serial.print("   Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    } else {
        Serial.println();
        Serial.println("❌ WiFi connection failed!");
        Serial.println("   Check SSID/password and retry.");
    }
}

void connectMQTT() {
    Serial.print("🔗 Connecting to MQTT broker: ");
    Serial.println(MQTT_BROKER);
    
    // Attempt connection
    bool connected;
    if (strlen(MQTT_USERNAME) > 0) {
        connected = mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
    } else {
        connected = mqttClient.connect(MQTT_CLIENT_ID);
    }
    
    if (connected) {
        Serial.println("✅ MQTT connected!");
        
        // Subscribe to command topic
        mqttClient.subscribe(MQTT_TOPIC_COMMANDS);
        Serial.print("   Subscribed to: ");
        Serial.println(MQTT_TOPIC_COMMANDS);
    } else {
        Serial.print("❌ MQTT connection failed, rc=");
        Serial.println(mqttClient.state());
    }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("📨 Message received [");
    Serial.print(topic);
    Serial.print("]: ");
    
    String message = "";
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    
    // Handle commands
    if (String(topic) == MQTT_TOPIC_COMMANDS) {
        if (message == "status") {
            publishStatus();
        } else if (message == "restart") {
            Serial.println("🔄 Restarting...");
            ESP.restart();
        }
    }
}

void publishStatus() {
    if (!mqttClient.connected()) return;
    
    String status = "{";
    status += "\"agent\":\"" + String(AGENT_NAME) + "\",";
    status += "\"version\":\"" + String(VERSION) + "\",";
    status += "\"uptime\":" + String(millis() / 1000) + ",";
    status += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
    status += "\"wifi_rssi\":" + String(WiFi.RSSI());
    status += "}";
    
    mqttClient.publish(MQTT_TOPIC_STATUS, status.c_str());
    Serial.println("📤 Published status: " + status);
}

void publishSensorData() {
    if (!mqttClient.connected()) return;
    
    // Placeholder sensor data (replace with actual sensor readings)
    String data = "{";
    data += "\"timestamp\":" + String(millis()) + ",";
    data += "\"temperature\":25.0,";  // Replace with DHT22 reading
    data += "\"humidity\":60.0";      // Replace with DHT22 reading
    data += "}";
    
    mqttClient.publish(MQTT_TOPIC_SENSORS, data.c_str());
    Serial.println("📤 Published sensor data: " + data);
}
