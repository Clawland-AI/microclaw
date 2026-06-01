# MicroClaw

**Sensor-level micro AI Agent in C/Rust. <1MB RAM, runs on $2-5 MCU hardware.**

> Part of the [Clawland](https://github.com/Clawland-AI) ecosystem.

---

## Overview

MicroClaw is the smallest member of the Claw family — a bare-metal AI agent that runs on microcontrollers costing as little as $2. It handles real-time sensor reading, local decision-making, and upstream reporting to PicClaw or NanoClaw nodes.

## Key Features

- **Bare Metal** — No OS required, runs directly on MCU
- **Ultra-Low Power** — Sleep modes, wake-on-event, battery-friendly
- **Real-Time Sensing** — Sub-millisecond sensor polling and reaction
- **Local Rules Engine** — If-then-else + threshold-based decisions without cloud
- **Mesh Communication** — ESP-NOW, LoRa, Zigbee, or UART to upstream agents
- **OTA Updates** — Over-the-air firmware updates via PicClaw

## ESP32-C3 Port

This repo includes a PlatformIO-based ESP32-C3 port that turns MicroClaw into a battery-powered sensor node:

- DHT22 temperature/humidity driver with explicit read failure handling.
- MQTT reporting to `clawland/{node_id}/sensors/{sensor_type}`.
- Low-battery alerts to `clawland/{node_id}/alerts/critical`.
- Configurable wake -> read sensors -> report -> OTA window -> deep sleep cycle.
- ADC battery voltage monitoring through a 2:1 divider.
- Short ArduinoOTA maintenance window on every wake cycle.
- Wiring guide for DHT22, DS18B20, PIR, and relay modules.

### Configure

Edit `platformio.ini` build flags before flashing:

| Build flag | Purpose | Default |
| --- | --- | --- |
| `MICROCLAW_NODE_ID` | Stable node id used in MQTT topics | `microclaw-esp32c3-001` |
| `MICROCLAW_WIFI_SSID` | Wi-Fi network name | `CHANGE_ME` |
| `MICROCLAW_WIFI_PASSWORD` | Wi-Fi password | `CHANGE_ME` |
| `MICROCLAW_MQTT_HOST` | PicoClaw/NanoClaw MQTT broker host | `192.168.1.10` |
| `MICROCLAW_MQTT_PORT` | MQTT broker port | `1883` |
| `MICROCLAW_DHT_PIN` | DHT22 data GPIO | `4` |
| `MICROCLAW_BATTERY_PIN` | ADC GPIO for battery divider | `0` |
| `MICROCLAW_LOW_BATTERY_MV` | Critical alert threshold | `3300` |
| `MICROCLAW_SLEEP_SECONDS` | Deep-sleep interval | `300` |
| `MICROCLAW_OTA_WINDOW_SECONDS` | OTA maintenance window per wake | `20` |

### Build and Flash

```sh
platformio run -e esp32c3
platformio run -e esp32
platformio run -e esp32c3 -t upload
platformio device monitor -e esp32c3
```

### MQTT Payloads

Status and sensor reports are retained JSON messages on the `clawland/{node_id}/...` topic tree:

```json
{
  "node_id": "microclaw-esp32c3-001",
  "state": "awake",
  "battery_mv": 3812,
  "sleep_seconds": 300,
  "ota_window_seconds": 20,
  "firmware": "0.2.0"
}
```

```json
{
  "node_id": "microclaw-esp32c3-001",
  "sensor_type": "dht22",
  "temperature_c": 24.1,
  "humidity_percent": 61.4,
  "battery_mv": 3812,
  "firmware": "0.2.0"
}
```

Validation without ESP32 hardware:

```sh
node scripts/validate-esp32-port.mjs
```

See [`docs/wiring.md`](docs/wiring.md) for sensor wiring and [`docs/telemetry-contract.md`](docs/telemetry-contract.md) for the MQTT topic and payload contract.

## Supported Hardware

| MCU | RAM | Flash | Price | Notes |
|-----|-----|-------|-------|-------|
| ESP32-C3 | 400KB | 4MB | ~$2 | Wi-Fi + BLE, best value |
| ESP32-S3 | 512KB | 8MB | ~$3 | AI acceleration, camera support |
| STM32F103 | 20KB | 64KB | ~$1.5 | Ultra-low cost, bare essentials |
| RP2040 | 264KB | 2MB | ~$1 | Dual-core, PIO for custom protocols |
| nRF52840 | 256KB | 1MB | ~$4 | BLE mesh, ultra-low power |

## Architecture

```
┌───────────────────────────┐
│       MicroClaw (MCU)     │
│  ┌────────┐ ┌───────────┐│
│  │Sensors │ │Rules Engine││
│  │  ADC   │ │ Threshold  ││
│  │  GPIO  │ │ FSM        ││
│  └────────┘ └───────────┘│
│  ┌────────┐ ┌───────────┐│
│  │Comms   │ │Power Mgmt ││
│  │ESP-NOW │ │Deep Sleep  ││
│  │UART    │ │Wake Timer  ││
│  └────────┘ └───────────┘│
└───────────────────────────┘
        │ Report upstream
        ▼
   ┌─────────┐
   │ PicClaw │
   └─────────┘
```

## Status

🚧 **Pre-Alpha** — Architecture design phase. Looking for contributors!

## Contributing

See the [Clawland Contributing Guide](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTING.md).

**Core contributors share 20% of product revenue.** Read the [Contributor Revenue Share](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTOR-REVENUE-SHARE.md) terms.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
