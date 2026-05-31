#include "MicroClawMqtt.h"

MicroClawMqtt::MicroClawMqtt(const char* nodeId, const char* host, uint16_t port)
    : nodeId_(nodeId), host_(host), port_(port), mqtt_(wifi_) {}

void MicroClawMqtt::begin() {
  mqtt_.setServer(host_, port_);
  mqtt_.setBufferSize(512);
}

bool MicroClawMqtt::ensureConnected(uint32_t timeoutMs) {
  const uint32_t deadline = millis() + timeoutMs;
  while (!mqtt_.connected() && millis() < deadline) {
    if (mqtt_.connect(nodeId_)) {
      return true;
    }
    delay(400);
  }
  return mqtt_.connected();
}

bool MicroClawMqtt::publishSensor(const char* sensorType, const String& json) {
  const String suffix = String("sensors/") + sensorType;
  return ensureConnected() && mqtt_.publish(topic(suffix.c_str()).c_str(), json.c_str(), true);
}

bool MicroClawMqtt::publishAlert(const char* level, const String& json) {
  const String suffix = String("alerts/") + level;
  return ensureConnected() && mqtt_.publish(topic(suffix.c_str()).c_str(), json.c_str(), true);
}

void MicroClawMqtt::loop() {
  mqtt_.loop();
}

String MicroClawMqtt::topic(const char* suffix) const {
  return String("clawland/") + nodeId_ + "/" + suffix;
}
