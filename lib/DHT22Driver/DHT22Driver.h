#ifndef DHT22DRIVER_H
#define DHT22DRIVER_H

#include <Arduino.h>
#include <DHT.h>

/**
 * DHT22Driver - Temperature and Humidity Sensor Driver
 * 
 * Wrapper around Adafruit DHT library with error handling,
 * JSON output, and configurable retry logic.
 * 
 * Usage:
 *   DHT22Driver sensor(4);  // GPIO 4
 *   sensor.begin();
 *   
 *   float temp = sensor.readTemperature();
 *   float humidity = sensor.readHumidity();
 *   
 *   String json = sensor.readJSON();
 */
class DHT22Driver {
public:
    /**
     * Constructor
     * @param pin GPIO pin connected to DHT22 DATA pin
     * @param retries Number of read retries on failure (default: 3)
     * @param retryDelay Delay between retries in ms (default: 2000)
     */
    DHT22Driver(uint8_t pin, uint8_t retries = 3, uint16_t retryDelay = 2000);
    
    /**
     * Initialize the sensor
     * Call this in setup()
     */
    void begin();
    
    /**
     * Read temperature in Celsius
     * @return Temperature or NaN on error
     */
    float readTemperature();
    
    /**
     * Read temperature in Fahrenheit
     * @return Temperature or NaN on error
     */
    float readTemperatureF();
    
    /**
     * Read relative humidity (%)
     * @return Humidity or NaN on error
     */
    float readHumidity();
    
    /**
     * Read temperature and humidity together (more efficient)
     * @param temp Output parameter for temperature (C)
     * @param humidity Output parameter for humidity (%)
     * @return true on success, false on error
     */
    bool read(float &temp, float &humidity);
    
    /**
     * Read temperature and humidity and return as JSON
     * Format: {"temperature":25.0,"humidity":60.0,"status":"ok"}
     * or: {"error":"Sensor read failed","status":"error"}
     * @return JSON string
     */
    String readJSON();
    
    /**
     * Check if last read was successful
     * @return true if last read succeeded
     */
    bool isOK();
    
    /**
     * Get error message from last failed read
     * @return Error message or empty string if no error
     */
    String getError();
    
private:
    DHT dht;
    uint8_t pin;
    uint8_t maxRetries;
    uint16_t retryDelay;
    bool lastReadOK;
    String lastError;
    
    /**
     * Internal read with retry logic
     * @param temp Output parameter for temperature
     * @param humidity Output parameter for humidity
     * @return true on success, false after all retries exhausted
     */
    bool readWithRetry(float &temp, float &humidity);
};

#endif // DHT22DRIVER_H
