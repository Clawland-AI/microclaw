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

The PlatformIO firmware now supports both `esp32c3` and legacy `esp32` build
targets. Runtime defaults are credential-free/no-op safe: Wi-Fi, MQTT, OTA, and
deep sleep are configured through build flags rather than committed secrets.

See `docs/esp32-c3-port.md` for pin defaults, MQTT payload shape, build
commands, OTA/deep-sleep flags, and hardware validation notes.

## Contributing

See the [Clawland Contributing Guide](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTING.md).

**Core contributors share 20% of product revenue.** Read the [Contributor Revenue Share](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTOR-REVENUE-SHARE.md) terms.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
