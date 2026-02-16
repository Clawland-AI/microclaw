/**
 * DHT22 Example - Basic Temperature and Humidity Reading
 * 
 * Demonstrates basic usage of DHT22Driver with error handling.
 */
#include <Arduino.h>
#include <DHT22Driver.h>

// Initialize DHT22 on GPIO 4
// Default: 3 retries with 2-second delay
DHT22Driver sensor(4);

void setup() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println("========================================");
    Serial.println("🌡️  DHT22 Example - MicroClaw");
    Serial.println("========================================");
    
    // Initialize sensor
    sensor.begin();
    
    Serial.println("✅ Sensor initialized");
    Serial.println("⏳ Waiting 2 seconds for sensor warmup...");
    delay(2000);
}

void loop() {
    Serial.println("\n--- Reading DHT22 ---");
    
    // Method 1: Read temperature and humidity separately
    float temp = sensor.readTemperature();
    float humidity = sensor.readHumidity();
    
    if (sensor.isOK()) {
        Serial.printf("🌡️  Temperature: %.1f°C (%.1f°F)\n", temp, temp * 9.0/5.0 + 32.0);
        Serial.printf("💧 Humidity: %.1f%%\n", humidity);
        
        // Calculate heat index (feels-like temperature)
        float heatIndex = computeHeatIndex(temp, humidity);
        Serial.printf("🔥 Heat Index: %.1f°C\n", heatIndex);
        
        // Comfort level
        String comfort = getComfortLevel(temp, humidity);
        Serial.printf("😊 Comfort: %s\n", comfort.c_str());
    } else {
        Serial.println("❌ Error: " + sensor.getError());
    }
    
    // Method 2: Read as JSON (for MQTT/HTTP reporting)
    Serial.println("\n--- JSON Output ---");
    String json = sensor.readJSON();
    Serial.println(json);
    
    // Wait 5 seconds before next reading
    // DHT22 sensor needs 2+ seconds between reads
    delay(5000);
}

/**
 * Compute heat index (feels-like temperature)
 * Simplified Steadman formula
 */
float computeHeatIndex(float temp, float humidity) {
    float T = temp;
    float RH = humidity;
    
    float HI = -8.78469475556 + 
               1.61139411 * T + 
               2.33854883889 * RH + 
               -0.14611605 * T * RH + 
               -0.012308094 * T * T + 
               -0.0164248277778 * RH * RH + 
               0.002211732 * T * T * RH + 
               0.00072546 * T * RH * RH + 
               -0.000003582 * T * T * RH * RH;
    
    return HI;
}

/**
 * Get comfort level based on temperature and humidity
 */
String getComfortLevel(float temp, float humidity) {
    if (temp < 15.0) {
        return "Cold";
    } else if (temp > 30.0) {
        return "Hot";
    } else if (humidity < 30.0) {
        return "Dry";
    } else if (humidity > 70.0) {
        return "Humid";
    } else {
        return "Comfortable";
    }
}
