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

## ESP32-C3 Firmware Port

The default PlatformIO target is now `esp32c3`, using the
`esp32-c3-devkitm-1` board profile. The legacy `esp32` environment remains
available for generic ESP32 dev boards.

The firmware reads:

- one analog sensor on `MICROCLAW_ANALOG_PIN` (default `GPIO2` for ESP32-C3),
- one DHT-style temperature/humidity sensor on `MICROCLAW_DHT_PIN` (default
  `GPIO4`, `DHT22`),
- Wi-Fi RSSI and uptime.

When MQTT is configured, the node publishes compact JSON status payloads to
`MICROCLAW_MQTT_TOPIC`. When Wi-Fi is connected, ArduinoOTA is enabled by
default so test devices can receive firmware updates without a USB cable.

### Configuration

Configure credentials and endpoints with PlatformIO build flags or your local
environment. Do not commit private SSIDs, passwords, broker credentials, or OTA
passwords.

Example local build flags:

```ini
build_flags =
    -D MICROCLAW_NODE_ID=\"greenhouse-node-01\"
    -D MICROCLAW_WIFI_SSID=\"your-ssid\"
    -D MICROCLAW_WIFI_PASSWORD=\"your-password\"
    -D MICROCLAW_MQTT_HOST=\"192.0.2.10\"
    -D MICROCLAW_MQTT_PORT=1883
    -D MICROCLAW_MQTT_TOPIC=\"clawland/microclaw/greenhouse-node-01/status\"
```

Useful optional flags:

```ini
    -D MICROCLAW_ANALOG_PIN=2
    -D MICROCLAW_DHT_PIN=4
    -D MICROCLAW_DHT_TYPE=DHT22
    -D MICROCLAW_REPORT_INTERVAL_MS=10000
    -D MICROCLAW_ENABLE_OTA=1
```

### Validation

```bash
pio run -e esp32c3
pio run -e esp32
```

Runtime validation requires physical ESP32-C3 hardware, a DHT-compatible
sensor, and an MQTT broker. Without hardware, the build verifies that the
firmware, dependencies, and board targets compile.

## Contributing

See the [Clawland Contributing Guide](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTING.md).

**Core contributors share 20% of product revenue.** Read the [Contributor Revenue Share](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTOR-REVENUE-SHARE.md) terms.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
