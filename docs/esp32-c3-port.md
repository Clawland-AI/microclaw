# MicroClaw ESP32-C3 Port

This firmware keeps both the legacy ESP32 dev-board target and the ESP32-C3
DevKitM-1 target buildable from the same source tree.

## Build targets

```bash
pio run -e esp32c3
pio run -e esp32
```

`esp32c3` is the default target. The `esp32` target remains available for the
existing ESP32 development environment.

## Configuration

All runtime secrets are safe no-op defaults. Configure local devices with
PlatformIO build flags or environment-specific overrides; do not commit real
SSIDs, passwords, broker addresses, or OTA passwords.

Common flags:

```ini
-D MICROCLAW_NODE_ID=\"greenhouse-node-01\"
-D MICROCLAW_WIFI_SSID=\"your-ssid\"
-D MICROCLAW_WIFI_PASSWORD=\"your-password\"
-D MICROCLAW_MQTT_HOST=\"192.0.2.10\"
-D MICROCLAW_MQTT_PORT=1883
-D MICROCLAW_MQTT_TOPIC=\"clawland/microclaw/greenhouse-node-01/status\"
-D MICROCLAW_ENABLE_OTA=1
-D MICROCLAW_OTA_PASSWORD=\"change-me\"
-D MICROCLAW_ENABLE_DEEP_SLEEP=1
-D MICROCLAW_SLEEP_SECONDS=300UL
```

## Default pins

| Signal | ESP32-C3 | ESP32 dev | Notes |
| --- | --- | --- | --- |
| Analog sensor | GPIO2 | GPIO34 | Raw ADC + millivolts in JSON payload. |
| DHT data | GPIO4 | GPIO4 | DHT22 default; override `MICROCLAW_DHT_TYPE`. |
| Battery sense | GPIO0 | GPIO35 | Assumes divider ratio from `MICROCLAW_BATTERY_DIVIDER_RATIO`. |

## MQTT payload

The firmware publishes compact JSON to `MICROCLAW_MQTT_TOPIC` when Wi-Fi and
MQTT are configured:

```json
{
  "nodeId": "microclaw-001",
  "agent": "microclaw",
  "version": "0.2.0",
  "board": "esp32-c3-devkitm-1",
  "uptimeMs": 10000,
  "analogRaw": 1234,
  "analogMv": 812,
  "batteryMv": 3900,
  "batteryLow": false,
  "temperatureC": 22.40,
  "humidityPct": 55.10,
  "dhtOk": true,
  "wifiRssi": -61
}
```

If DHT reading fails, `temperatureC` and `humidityPct` are `null` and `dhtOk`
is `false`.

## Hardware validation limits

Build validation confirms both firmware targets compile and fit. Runtime
validation still requires physical ESP32-C3 hardware, a configured DHT-style
sensor, optional battery divider, Wi-Fi, MQTT broker, and OTA test path.
