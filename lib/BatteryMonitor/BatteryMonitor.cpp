#include "BatteryMonitor.h"

BatteryMonitor::BatteryMonitor(uint8_t adcPin, float dividerRatio)
    : adcPin_(adcPin), dividerRatio_(dividerRatio) {}

void BatteryMonitor::begin() const {
  pinMode(adcPin_, INPUT);
  analogReadResolution(12);
}

uint16_t BatteryMonitor::readMillivolts(uint8_t samples) const {
  uint32_t total = 0;
  const uint8_t sampleCount = samples == 0 ? 1 : samples;
  for (uint8_t i = 0; i < sampleCount; i++) {
    total += analogReadMilliVolts(adcPin_);
    delay(4);
  }
  const float averageMv = static_cast<float>(total) / sampleCount;
  return static_cast<uint16_t>(averageMv * dividerRatio_);
}

bool BatteryMonitor::isLow(uint16_t millivolts, uint16_t thresholdMv) const {
  return millivolts > 0 && millivolts < thresholdMv;
}
