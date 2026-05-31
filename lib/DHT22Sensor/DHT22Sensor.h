#pragma once

#include <Arduino.h>
#include <DHT.h>

struct DHT22Reading {
  bool ok;
  float temperatureC;
  float humidityPercent;
  const char* error;
};

class DHT22Sensor {
 public:
  explicit DHT22Sensor(uint8_t pin);

  void begin();
  DHT22Reading read();

 private:
  DHT dht_;
};
