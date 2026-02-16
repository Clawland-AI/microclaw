#include "DHT22Driver.h"

DHT22Driver::DHT22Driver(uint8_t pin, uint8_t retries, uint16_t retryDelay)
    : dht(pin, DHT22), pin(pin), maxRetries(retries), retryDelay(retryDelay), lastReadOK(false), lastError("") {
}

void DHT22Driver::begin() {
    dht.begin();
    lastReadOK = false;
    lastError = "";
}

float DHT22Driver::readTemperature() {
    float temp, humidity;
    if (readWithRetry(temp, humidity)) {
        return temp;
    }
    return NAN;
}

float DHT22Driver::readTemperatureF() {
    float temp = readTemperature();
    if (!isnan(temp)) {
        return temp * 9.0 / 5.0 + 32.0;
    }
    return NAN;
}

float DHT22Driver::readHumidity() {
    float temp, humidity;
    if (readWithRetry(temp, humidity)) {
        return humidity;
    }
    return NAN;
}

bool DHT22Driver::read(float &temp, float &humidity) {
    return readWithRetry(temp, humidity);
}

String DHT22Driver::readJSON() {
    float temp, humidity;
    
    if (readWithRetry(temp, humidity)) {
        String json = "{";
        json += "\"temperature\":" + String(temp, 1) + ",";
        json += "\"humidity\":" + String(humidity, 1) + ",";
        json += "\"status\":\"ok\"";
        json += "}";
        return json;
    } else {
        String json = "{";
        json += "\"error\":\"" + lastError + "\",";
        json += "\"status\":\"error\"";
        json += "}";
        return json;
    }
}

bool DHT22Driver::isOK() {
    return lastReadOK;
}

String DHT22Driver::getError() {
    return lastError;
}

bool DHT22Driver::readWithRetry(float &temp, float &humidity) {
    for (uint8_t attempt = 0; attempt < maxRetries; attempt++) {
        // Read from sensor
        humidity = dht.readHumidity();
        temp = dht.readTemperature();
        
        // Check if read succeeded
        if (!isnan(humidity) && !isnan(temp)) {
            // Validate reasonable ranges
            if (temp < -40.0 || temp > 80.0) {
                lastError = "Temperature out of range: " + String(temp, 1) + "C";
                lastReadOK = false;
                
                if (attempt < maxRetries - 1) {
                    delay(retryDelay);
                    continue;
                }
                return false;
            }
            
            if (humidity < 0.0 || humidity > 100.0) {
                lastError = "Humidity out of range: " + String(humidity, 1) + "%";
                lastReadOK = false;
                
                if (attempt < maxRetries - 1) {
                    delay(retryDelay);
                    continue;
                }
                return false;
            }
            
            // Success!
            lastReadOK = true;
            lastError = "";
            return true;
        }
        
        // Read failed, set error message
        if (isnan(humidity) && isnan(temp)) {
            lastError = "Sensor read failed (both values NaN)";
        } else if (isnan(humidity)) {
            lastError = "Humidity read failed (NaN)";
        } else {
            lastError = "Temperature read failed (NaN)";
        }
        
        lastReadOK = false;
        
        // Retry if not last attempt
        if (attempt < maxRetries - 1) {
            delay(retryDelay);
        }
    }
    
    // All retries exhausted
    lastError += " after " + String(maxRetries) + " attempts";
    return false;
}
