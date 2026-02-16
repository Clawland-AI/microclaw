# 🍇 MicroClaw — MCU-level Sensor Agent

**MicroClaw** is the L3 (sensor-level) agent in the Clawland edge AI network. It runs on ESP32 microcontrollers with <1MB RAM and $2-5 hardware cost, collecting sensor data and reporting to NanoClaw (L2) or PicoClaw (L1) gateways via MQTT.

## Features

- ✅ **ESP32 PlatformIO project** — ready for upload
- ✅ **DHT22 temperature/humidity sensor** — accurate environmental monitoring
- ✅ **MQTT client** — reports to NanoClaw/PicoClaw gateways
- ✅ **Auto-reconnect** — resilient to network interruptions
- ✅ **Deep sleep mode** — battery-powered operation (up to 6 months on 3xAA)
- ✅ **Configurable publish interval** — balance power vs. data freshness

## Hardware Requirements

- **MCU:** ESP32-DevKitC or compatible (ESP-WROOM-32)
- **Sensor:** DHT22 (AM2302) temperature/humidity sensor
- **Power:** 5V USB or 3xAA battery pack (4.5V via LDO)
- **Wiring:**
  - DHT22 VCC → ESP32 3.3V
  - DHT22 GND → ESP32 GND
  - DHT22 DATA → ESP32 GPIO4 (with 10kΩ pull-up resistor)

## Quick Start

### 1. Clone and Configure

```bash
git clone https://github.com/Clawland-AI/microclaw.git
cd microclaw
```

### 2. Update Configuration

Edit `src/main.cpp` and set your credentials:

```cpp
// WiFi Configuration
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// MQTT Configuration
const char* MQTT_BROKER = "mqtt.clawland.local";  // Your NanoClaw/PicoClaw gateway
const char* NODE_ID = "node_001";  // Unique identifier for this MicroClaw
```

### 3. Upload Firmware

```bash
# Install PlatformIO if not already installed
pip install platformio

# Build and upload
pio run --target upload

# Monitor serial output
pio device monitor
```

## MQTT Topics

MicroClaw publishes to the following topics (format: `clawland/{node_id}/sensors/{sensor_type}`):

- **Temperature:** `clawland/node_001/sensors/temperature`
  ```json
  {"value": 23.50, "unit": "C"}
  ```

- **Humidity:** `clawland/node_001/sensors/humidity`
  ```json
  {"value": 65.20, "unit": "%"}
  ```

- **Status:** `clawland/node_001/status`
  ```json
  {"status": "online", "version": "0.2.0"}
  ```

## Configuration Options

| Parameter | Description | Default |
|-----------|-------------|---------|
| `MQTT_BROKER` | Broker hostname/IP | `mqtt.clawland.local` |
| `MQTT_PORT` | Broker port | `1883` |
| `NODE_ID` | Unique node identifier | `node_001` |
| `PUBLISH_INTERVAL` | Publish frequency (ms) | `60000` (60 sec) |
| `DHT_PIN` | GPIO pin for DHT22 | `4` |

## Auto-Reconnect

MicroClaw automatically handles:

- **WiFi reconnection** — retries on connection loss
- **MQTT reconnection** — exponential backoff (5 sec intervals)
- **Graceful degradation** — continues operating even if MQTT is unavailable

## Deep Sleep Mode

For battery-powered deployments, MicroClaw supports deep sleep:

```cpp
// Wake every 10 minutes, read sensor, publish, then sleep
const int SLEEP_DURATION = 10 * 60 * 1000000;  // 10 minutes in microseconds
esp_deep_sleep(SLEEP_DURATION);
```

**Battery life estimate:**
- **Active mode:** ~100mA → 10 hours per AA battery
- **Deep sleep mode:** ~10µA → 6+ months per AA battery (10min wake interval)

## Troubleshooting

### MQTT Connection Fails

1. Verify broker is reachable: `ping mqtt.clawland.local`
2. Check firewall rules (port 1883 open)
3. Enable debug output in `platformio.ini`:
   ```ini
   build_flags = -DMQTT_DEBUG
   ```

### DHT22 Reads NaN

1. Check wiring (10kΩ pull-up resistor required)
2. Verify sensor power (3.3V, not 5V)
3. Increase read delay in code (DHT22 needs 2 sec between reads)

### WiFi Won't Connect

1. Verify SSID/password in `main.cpp`
2. Check WiFi signal strength (ESP32 needs >-70dBm)
3. Some enterprise WiFi networks block IoT devices (use 2.4GHz, not 5GHz)

## Development

### Adding New Sensors

1. Add library to `platformio.ini`:
   ```ini
   lib_deps =
       your-sensor-library@^1.0
   ```

2. Create topic in `main.cpp`:
   ```cpp
   const char* TOPIC_NEW_SENSOR = "clawland/node_001/sensors/new_sensor";
   ```

3. Publish in `publishSensorData()`:
   ```cpp
   float reading = sensor.read();
   char payload[50];
   snprintf(payload, sizeof(payload), "{\"value\":%.2f}", reading);
   mqttClient.publish(TOPIC_NEW_SENSOR, payload);
   ```

## Architecture

```
┌─────────────────────────────────────────────┐
│  Clawland Edge AI Network                   │
├─────────────────────────────────────────────┤
│  L3 MicroClaw (ESP32)                       │
│    ├─ DHT22 Sensor                          │
│    ├─ MQTT Client                           │
│    └─ Deep Sleep                            │
│           ↓ MQTT                            │
│  L2 NanoClaw (Raspberry Pi)                 │
│    ├─ Sensor Aggregation                    │
│    ├─ Local Decision Engine                 │
│    └─ LoRa Gateway (optional)               │
│           ↓ MQTT                            │
│  L1 PicoClaw (Raspberry Pi 5)               │
│    ├─ Regional AI Orchestration             │
│    ├─ Model Inference                       │
│    └─ Fleet Management API                  │
│           ↓ HTTPS                           │
│  L0 MoltClaw (Cloud)                        │
│    └─ Global Coordination                   │
└─────────────────────────────────────────────┘
```

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for development setup, coding standards, and PR guidelines.

## License

MIT License — see [LICENSE](LICENSE) for details.

## Links

- **Clawland Docs:** https://docs.clawland.ai
- **Hardware Guide:** [Hardware Wiring Guide](docs/hardware-wiring.md)
- **Issues:** https://github.com/Clawland-AI/microclaw/issues
- **Discussions:** https://github.com/Clawland-AI/microclaw/discussions
