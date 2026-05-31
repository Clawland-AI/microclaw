#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

class MicroClawMqtt {
 public:
  MicroClawMqtt(const char* nodeId, const char* host, uint16_t port);

  void begin();
  bool ensureConnected(uint32_t timeoutMs = 8000);
  bool publishSensor(const char* sensorType, const String& json);
  bool publishAlert(const char* level, const String& json);
  void loop();

 private:
  String topic(const char* suffix) const;

  const char* nodeId_;
  const char* host_;
  uint16_t port_;
  WiFiClient wifi_;
  PubSubClient mqtt_;
};
