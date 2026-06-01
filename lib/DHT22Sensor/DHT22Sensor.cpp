#include "DHT22Sensor.h"

DHT22Sensor::DHT22Sensor(uint8_t pin) : dht_(pin, DHT22) {}

void DHT22Sensor::begin() {
  dht_.begin();
}

DHT22Reading DHT22Sensor::read() {
  const float humidity = dht_.readHumidity();
  const float temperature = dht_.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    return {false, 0.0f, 0.0f, "dht22_read_failed"};
  }
  return {true, temperature, humidity, ""};
}
