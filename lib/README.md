# Sensor Driver Libraries

This directory contains custom sensor driver libraries for MicroClaw.

## Structure

Each sensor driver should be in its own subdirectory:

```
lib/
├── README.md (this file)
├── DHT22Driver/
│   ├── DHT22Driver.h
│   ├── DHT22Driver.cpp
│   └── examples/
├── BME280Driver/
│   ├── BME280Driver.h
│   ├── BME280Driver.cpp
│   └── examples/
└── ...
```

## Usage

PlatformIO will automatically discover libraries in this directory and make them available to `#include` in `src/main.cpp`.

## Creating a New Driver

1. Create a new subdirectory with your sensor name
2. Add `.h` and `.cpp` files with your driver code
3. Optionally add an `examples/` folder with usage examples
4. Include the header in `main.cpp` with: `#include <DriverName/DriverName.h>`

## Pre-installed Libraries

The following libraries are installed via `platformio.ini` and don't need to be added here:

- **PubSubClient** (MQTT client)
- **DHT sensor library** (Adafruit DHT sensors)
- **Adafruit Unified Sensor** (Common sensor interface)

Add custom drivers here when you need:
- Modified versions of existing libraries
- Drivers for sensors not in the PlatformIO registry
- Project-specific sensor abstractions
