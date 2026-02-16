# Deep Sleep Example

This example demonstrates ESP32 deep sleep for battery-powered MicroClaw operation.

## Features

- **Ultra-low power**: ~30µA in deep sleep (vs ~160mA awake)
- **Timer wakeup**: Configurable interval (default 5 minutes)
- **Battery monitoring**: ADC-based voltage reading with low-battery protection
- **Fast boot**: WiFi reconnect in ~2 seconds
- **MQTT reporting**: Publish sensor data, then sleep
- **Boot counter**: RTC memory persists across sleep cycles

## Expected Battery Life

With 1000mAh LiPo battery:

| Sleep Interval | Average Current | Battery Life |
|----------------|-----------------|--------------|
| 5 minutes      | 2.65 mA         | 15.7 days    |
| 15 minutes     | 0.95 mA         | 1.5 months   |
| 1 hour         | 0.25 mA         | 5.5 months   |

See `deep_sleep.ino` for detailed calculation.

## Wiring

### Battery Connection

```
LiPo 3.7V (1000-2000mAh)
   │
   ├─ ESP32 3.3V pin (or VIN with voltage regulator)
   │
   └─ Voltage divider for ADC:
      Battery+ ──[100kΩ]──+──[100kΩ]── GND
                          │
                       GPIO 35 (ADC)
```

### Voltage Divider Calculation

```
Battery: 4.2V max (fully charged LiPo)
ADC max: 3.3V (ESP32 limit)

With 2:1 divider (100kΩ + 100kΩ):
  ADC voltage = Battery / 2
  4.2V → 2.1V (safe for ESP32)
  3.0V → 1.5V (low battery threshold)
```

## Configuration

Edit `deep_sleep.ino` and change:

```cpp
const char* WIFI_SSID = "your_wifi_ssid";
const char* WIFI_PASSWORD = "your_wifi_password";
const char* MQTT_BROKER = "mqtt.example.com";

#define SLEEP_INTERVAL_SECONDS 300  // 5 minutes
#define LOW_BATTERY_THRESHOLD 3.2   // Volts
```

## Flashing

```bash
# Navigate to this directory
cd examples/deep_sleep

# Upload via PlatformIO
pio run --target upload

# Or use Arduino IDE:
# 1. Open deep_sleep.ino
# 2. Select board: ESP32 Dev Module
# 3. Upload
```

## Monitoring

```bash
# Watch serial output (baud: 115200)
pio device monitor

# Or use screen:
screen /dev/ttyUSB0 115200
```

Expected output:

```
========================================
🐾 MicroClaw Deep Sleep Example
   Boot #1
========================================
⏰ Wakeup: Unknown reason 0
🔋 Battery: 3.85V
📡 Connecting to WiFi... Connected!
   IP: 192.168.1.100, RSSI: -52 dBm
🔗 Connecting to MQTT... Connected!
📤 Publishing sensor data...
✅ Publish successful!
   Data: {"boot_count":1,"temperature":25.0,...}

💤 Entering deep sleep for 300 seconds...
   Current draw: ~10-50µA (vs ~160mA awake)
   Next wakeup: Timer
```

## Troubleshooting

### ESP32 won't wake up

**Cause**: EN pin pulled LOW externally  
**Fix**: Remove external connections to EN pin

### Battery drains quickly

**Possible causes**:
- WiFi takes too long to connect (>10s)
- MQTT broker unreachable (hangs)
- Sensors not powered down

**Fixes**:
- Reduce WiFi timeout to 10s
- Add MQTT connection timeout
- Turn off sensor power via GPIO before sleep:
  ```cpp
  digitalWrite(SENSOR_POWER_PIN, LOW);
  ```

### ADC reads incorrect voltage

**Cause**: No voltage divider or wrong ratio  
**Fix**: Use 2:1 divider (100kΩ + 100kΩ) and adjust multiplier:
```cpp
float voltage = (adcValue / 4095.0) * 3.3 * 2.0;  // *2.0 for 2:1 divider
```

### Boot loop (constant resets)

**Cause**: Low battery voltage (brownout detector)  
**Fix**: Charge battery or disable brownout:
```cpp
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Disable brownout detector
    // ... rest of setup
}
```

## Power Optimization Tips

### 1. Minimize Awake Time

- Use static IP to skip DHCP (~1s saved)
- Cache WiFi credentials (ESP32 does this automatically)
- Disable mDNS/DNS lookups

### 2. Reduce TX Power

```cpp
WiFi.setTxPower(WIFI_POWER_11dBm);  // Reduce from 20dBm (default)
```

### 3. External RTC Module

Use DS3231 RTC to wake ESP32 via external interrupt (saves even more power).

### 4. Sensor Power Control

```cpp
#define SENSOR_POWER_PIN 25

pinMode(SENSOR_POWER_PIN, OUTPUT);
digitalWrite(SENSOR_POWER_PIN, HIGH);  // Turn on sensors
delay(500);  // Sensor warmup
// ... read sensors
digitalWrite(SENSOR_POWER_PIN, LOW);   // Turn off before sleep
```

## Next Steps

- Integrate DHT22/DS18B20 sensor readings (see `../dht22_example/`)
- Add external wakeup (GPIO button or PIR sensor)
- Implement OTA updates (wake on demand)
- Use RTC memory for configuration persistence

## References

- [ESP32 Deep Sleep API](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html)
- [ESP32 Power Consumption](https://lastminuteengineers.com/esp32-sleep-modes-power-consumption/)
