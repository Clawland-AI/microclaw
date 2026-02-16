/**
 * MicroClaw Deep Sleep Example
 * 
 * Demonstrates battery-efficient operation:
 * - Wake from deep sleep
 * - Connect WiFi (fast reconnect)
 * - Read sensors (placeholder)
 * - Send MQTT report
 * - Go back to deep sleep
 * 
 * Expected battery life: 6-12 months on 1000mAh LiPo
 * with 5-minute sleep intervals.
 */
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// Configuration
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";
const char* MQTT_BROKER = "mqtt.example.com";
const int MQTT_PORT = 1883;
const char* MQTT_TOPIC = "microclaw/deep_sleep";

// Deep sleep configuration
#define SLEEP_INTERVAL_SECONDS 300  // 5 minutes (adjustable)
#define uS_TO_S_FACTOR 1000000ULL   // Microseconds to seconds
#define BATTERY_ADC_PIN 35          // GPIO 35 for battery voltage monitoring
#define LOW_BATTERY_THRESHOLD 3.2   // Volts (for 3.7V LiPo)

// Globals
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
RTC_DATA_ATTR int bootCount = 0;  // Persists across deep sleep

// Function declarations
void setupWiFi();
bool connectMQTT();
float readBatteryVoltage();
void publishSensorData();
void enterDeepSleep();

void setup() {
    Serial.begin(115200);
    delay(100);
    
    // Increment boot counter
    bootCount++;
    
    Serial.println("========================================");
    Serial.printf("🐾 MicroClaw Deep Sleep Example\n");
    Serial.printf("   Boot #%d\n", bootCount);
    Serial.println("========================================");
    
    // Print wakeup reason
    esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
    switch (wakeup_reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("⏰ Wakeup: Timer");
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("⏰ Wakeup: External signal (GPIO)");
            break;
        default:
            Serial.printf("⏰ Wakeup: Unknown reason %d\n", wakeup_reason);
            break;
    }
    
    // Check battery voltage
    float batteryVoltage = readBatteryVoltage();
    Serial.printf("🔋 Battery: %.2fV\n", batteryVoltage);
    
    if (batteryVoltage < LOW_BATTERY_THRESHOLD) {
        Serial.println("⚠️  LOW BATTERY! Extending sleep interval...");
        esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_SECONDS * 2 * uS_TO_S_FACTOR);
        esp_deep_sleep_start();  // Go to sleep immediately
    }
    
    // Connect WiFi (fast reconnect ~2s)
    setupWiFi();
    
    // Setup MQTT
    mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
    
    // Connect to MQTT with retries
    int attempts = 0;
    while (!connectMQTT() && attempts < 3) {
        Serial.println("⚠️  MQTT connection failed, retrying...");
        delay(1000);
        attempts++;
    }
    
    if (mqttClient.connected()) {
        // Publish sensor data
        publishSensorData();
        
        // Wait for message to send
        delay(500);
        mqttClient.disconnect();
    } else {
        Serial.println("❌ MQTT unavailable, skipping publish");
    }
    
    // Disconnect WiFi to save power
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    
    // Enter deep sleep
    enterDeepSleep();
}

void loop() {
    // Not used in deep sleep mode
}

void setupWiFi() {
    Serial.print("📡 Connecting to WiFi...");
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
        delay(500);
        Serial.print(".");
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println(" Connected!");
        Serial.printf("   IP: %s, RSSI: %d dBm\n", 
                      WiFi.localIP().toString().c_str(), 
                      WiFi.RSSI());
    } else {
        Serial.println(" Failed!");
    }
}

bool connectMQTT() {
    Serial.print("🔗 Connecting to MQTT...");
    
    String clientId = "microclaw-" + String(ESP.getEfuseMac());
    if (mqttClient.connect(clientId.c_str())) {
        Serial.println(" Connected!");
        return true;
    } else {
        Serial.printf(" Failed (rc=%d)\n", mqttClient.state());
        return false;
    }
}

float readBatteryVoltage() {
    // Read ADC value (0-4095 for 12-bit ADC)
    int adcValue = analogRead(BATTERY_ADC_PIN);
    
    // Convert to voltage
    // ESP32 ADC: 0-4095 → 0-3.3V (with voltage divider if needed)
    // For LiPo (3.7V nominal, 4.2V max), use 2:1 voltage divider:
    // Battery ---[100kΩ]---+---[100kΩ]--- GND
    //                      |
    //                   GPIO 35
    // So measured voltage = battery voltage / 2
    float voltage = (adcValue / 4095.0) * 3.3 * 2.0;  // Adjust multiplier for your divider
    
    return voltage;
}

void publishSensorData() {
    Serial.println("📤 Publishing sensor data...");
    
    // Read sensors (placeholder - replace with actual sensor code)
    float temperature = 25.0;  // Replace with DHT22/DS18B20 reading
    float humidity = 60.0;     // Replace with DHT22 reading
    float batteryVoltage = readBatteryVoltage();
    
    // Build JSON payload
    String payload = "{";
    payload += "\"boot_count\":" + String(bootCount) + ",";
    payload += "\"temperature\":" + String(temperature, 1) + ",";
    payload += "\"humidity\":" + String(humidity, 1) + ",";
    payload += "\"battery_voltage\":" + String(batteryVoltage, 2) + ",";
    payload += "\"uptime\":" + String(millis() / 1000) + ",";
    payload += "\"wifi_rssi\":" + String(WiFi.RSSI());
    payload += "}";
    
    if (mqttClient.publish(MQTT_TOPIC, payload.c_str())) {
        Serial.println("✅ Publish successful!");
        Serial.println("   Data: " + payload);
    } else {
        Serial.println("❌ Publish failed!");
    }
}

void enterDeepSleep() {
    Serial.printf("\n💤 Entering deep sleep for %d seconds...\n", SLEEP_INTERVAL_SECONDS);
    Serial.println("   Current draw: ~10-50µA (vs ~160mA awake)");
    Serial.println("   Next wakeup: Timer");
    Serial.flush();  // Wait for serial output to finish
    
    // Configure wakeup source: timer
    esp_sleep_enable_timer_wakeup(SLEEP_INTERVAL_SECONDS * uS_TO_S_FACTOR);
    
    // Optional: Disable WiFi and BT power during sleep (default)
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    
    // Enter deep sleep (will reset ESP32 on wakeup)
    esp_deep_sleep_start();
}

/*
 * Battery Life Calculation
 * ========================
 * 
 * Assumptions:
 * - LiPo battery: 1000mAh (3.7V nominal)
 * - Sleep current: 30µA (deep sleep with RTC)
 * - Awake current: 160mA (WiFi + MQTT)
 * - Awake time: 5 seconds (connect + publish + disconnect)
 * - Sleep time: 300 seconds (5 minutes)
 * 
 * Average current per cycle:
 *   = (160mA * 5s + 0.03mA * 300s) / 305s
 *   = (800 + 9) / 305
 *   = 2.65 mA
 * 
 * Expected battery life:
 *   = 1000mAh / 2.65mA
 *   = 377 hours
 *   = 15.7 days
 * 
 * With 15-minute sleep interval:
 *   Average current: 0.95 mA
 *   Battery life: 1053 hours = 43.8 days (~1.5 months)
 * 
 * With 1-hour sleep interval:
 *   Average current: 0.25 mA
 *   Battery life: 4000 hours = 166 days (~5.5 months)
 */
