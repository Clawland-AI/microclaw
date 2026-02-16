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

---

## Getting Started

### Prerequisites

- **PlatformIO** — Install via [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) or [CLI](https://docs.platformio.org/en/latest/core/installation/index.html)
- **ESP32 Development Board** — ESP32-DevKitC, ESP32-C3, or compatible
- **USB Cable** — For flashing and serial monitoring

### Installation

1. **Clone the repository**:
   ```bash
   git clone https://github.com/Clawland-AI/microclaw.git
   cd microclaw
   ```

2. **Configure WiFi and MQTT**:
   Edit `src/main.cpp` and replace the placeholder credentials:
   ```cpp
   const char* WIFI_SSID = "your_wifi_ssid";
   const char* WIFI_PASSWORD = "your_wifi_password";
   const char* MQTT_BROKER = "mqtt.example.com";
   ```

3. **Install dependencies** (automatic):
   ```bash
   pio lib install
   ```

### Flashing Instructions

#### Option 1: PlatformIO IDE (VS Code)

1. Open the `microclaw` folder in VS Code with PlatformIO extension
2. Connect your ESP32 via USB
3. Click **PlatformIO: Upload** (arrow icon in the bottom toolbar)
4. Open **PlatformIO: Serial Monitor** (plug icon) to view logs

#### Option 2: PlatformIO CLI

1. Connect your ESP32 via USB
2. Build and upload:
   ```bash
   pio run --target upload
   ```
3. Monitor serial output:
   ```bash
   pio device monitor
   ```

#### Option 3: Manual Flashing (esptool.py)

If PlatformIO is unavailable, use `esptool.py`:

```bash
# Build firmware
pio run

# Find COM port (Linux: /dev/ttyUSB0, macOS: /dev/cu.usbserial-*, Windows: COM3)
ls /dev/tty*

# Flash firmware
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z 0x1000 .pio/build/esp32/bootloader.bin \
  0x8000 .pio/build/esp32/partitions.bin \
  0x10000 .pio/build/esp32/firmware.bin

# Monitor serial
minicom -D /dev/ttyUSB0 -b 115200
```

### Troubleshooting

#### ESP32 not detected

```bash
# Check USB connection
lsusb  # Linux
system_profiler SPUSBDataType  # macOS

# Add user to dialout group (Linux)
sudo usermod -a -G dialout $USER
# Log out and back in
```

#### Upload fails

```bash
# Hold BOOT button on ESP32 while clicking Upload
# Or use slower baud rate
pio run --target upload --upload-port /dev/ttyUSB0 --upload-speed 115200
```

#### WiFi connection fails

- Verify SSID/password are correct
- Check 2.4GHz network (ESP32 doesn't support 5GHz)
- Ensure network allows new devices

#### MQTT connection fails

- Verify broker address and port
- Check firewall rules
- Test broker with `mosquitto_pub`:
  ```bash
  mosquitto_pub -h mqtt.example.com -t test -m "hello"
  ```

### Project Structure

```
microclaw/
├── platformio.ini       # PlatformIO config (board, libraries)
├── src/
│   └── main.cpp         # Main firmware code
├── lib/
│   └── README.md        # Custom sensor driver libraries
├── .gitignore           # Ignore build artifacts
└── README.md            # This file
```

### Next Steps

- **Add sensors**: See issue [#2 - DHT22 sensor driver](../../issues/2)
- **Enable MQTT**: Configure your broker in `main.cpp`
- **Low power mode**: See issue [#5 - Deep sleep](../../issues/5)
- **Hardware guide**: See issue [#4 - Wiring diagrams](../../issues/4)

---

## Status

🚧 **Pre-Alpha** — Architecture design phase. Looking for contributors!

## Contributing

See the [Clawland Contributing Guide](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTING.md).

**Core contributors share 20% of product revenue.** Read the [Contributor Revenue Share](https://github.com/Clawland-AI/.github/blob/main/CONTRIBUTOR-REVENUE-SHARE.md) terms.

## License

Apache License 2.0 — see [LICENSE](LICENSE) for details.
