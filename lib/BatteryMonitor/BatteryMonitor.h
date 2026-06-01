#pragma once

#include <Arduino.h>

class BatteryMonitor {
 public:
  BatteryMonitor(uint8_t adcPin, float dividerRatio);

  void begin() const;
  uint16_t readMillivolts(uint8_t samples = 8) const;
  bool isLow(uint16_t millivolts, uint16_t thresholdMv) const;

 private:
  uint8_t adcPin_;
  float dividerRatio_;
};
