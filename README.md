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

🚧 **Pre-Alpha** — The ESP32 reference firmware now includes a DHT22 sensor
loop, MQTT reporting, command handling, and optional deep sleep.

## MicroClaw ESP32 Sensor Agent

The default PlatformIO target builds an Arduino-based ESP32 firmware for a
low-cost DHT22 environmental node.

### Hardware

| Part | Default pin | Notes |
|------|-------------|-------|
| ESP32 dev board | - | `esp32dev` PlatformIO target |
| DHT22 data | GPIO 4 | Override with `MICROCLAW_DHT_PIN` |
| DHT22 VCC/GND | 3.3V/GND | Use a 4.7k-10k pull-up on DATA if the module does not include one |

### Configuration

Set deployment-specific values in `platformio.ini` `build_flags`:

```ini
build_flags =
    -DMICROCLAW_WIFI_SSID=\"lab-wifi\"
    -DMICROCLAW_WIFI_PASSWORD=\"secret\"
    -DMICROCLAW_MQTT_HOST=\"192.168.1.10\"
    -DMICROCLAW_MQTT_PORT=1883
    -DMICROCLAW_NODE_ID=\"rack-a1-dht22\"
    -DMICROCLAW_DHT_PIN=4
    -DMICROCLAW_WAKE_INTERVAL_SECONDS=60
    -DMICROCLAW_DEEP_SLEEP_ENABLED=1
```

### MQTT Topics

For `MICROCLAW_NODE_ID=rack-a1-dht22`, the firmware uses:

| Topic | Direction | Payload |
|-------|-----------|---------|
| `microclaw/rack-a1-dht22/telemetry` | publish | JSON temperature, humidity, thresholds, uptime, heap, alert flag |
| `microclaw/rack-a1-dht22/alert` | publish | Same JSON telemetry when thresholds are exceeded |
| `microclaw/rack-a1-dht22/status` | publish | Retained online/offline/sensor/command status |
| `microclaw/rack-a1-dht22/command` | subscribe | JSON command from PicClaw/NanoClaw |

Supported commands:

```json
{"type":"sample"}
{"type":"sleep"}
{"type":"set_thresholds","temp_alert_c":32.5,"humidity_alert_pct":75}
```

### Deep Sleep

When `MICROCLAW_DEEP_SLEEP_ENABLED=1`, MicroClaw publishes one sample, enables
`esp_sleep_enable_timer_wakeup`, and enters `esp_deep_sleep_start()` until the
next timer wake. This keeps battery deployments simple while preserving the
same MQTT telemetry shape as always-on nodes.

### Verification

Without a local ESP32 toolchain, run the static firmware contract tests:

```bash
python tests/test_firmware_static.py
```

With PlatformIO installed, run:

```bash
pio run
```

## Contributing

See the [Clawland Contributing Guide](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTING.md).

**Core contributors share 20% of product revenue.** Read the [Contributor Revenue Share](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTOR-REVENUE-SHARE.md) terms.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
