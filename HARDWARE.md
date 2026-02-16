# MicroClaw Hardware Wiring Guide

Complete wiring diagrams and pin reference for connecting sensors and actuators to ESP32.

---

## Table of Contents

- [Pin Reference](#pin-reference)
- [DHT22 Temperature & Humidity Sensor](#dht22-temperature--humidity-sensor)
- [DS18B20 Waterproof Temperature Sensor](#ds18b20-waterproof-temperature-sensor)
- [PIR Motion Sensor](#pir-motion-sensor)
- [Relay Module](#relay-module)
- [Power Supply Guidelines](#power-supply-guidelines)
- [Common Issues](#common-issues)

---

## Pin Reference

### ESP32 DevKit v1 / ESP32-WROOM-32

| GPIO | Function | Notes |
|------|----------|-------|
| **Power** |
| 3V3 | 3.3V output | Max 500mA, use for sensors |
| 5V | 5V input/output | From USB or external, use for relays |
| GND | Ground | Multiple GND pins available |
| **Input Only (no pullup)** |
| 34, 35, 36, 39 | ADC only | Good for analog sensors |
| **Strapping Pins (avoid if possible)** |
| 0, 2, 5, 12, 15 | Boot mode control | May cause boot issues if held LOW/HIGH |
| **Safe GPIO for sensors** |
| 4, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33 | General purpose | Best for DHT22, PIR, relays |
| **I2C (default)** |
| 21 | SDA | For BME280, OLED displays |
| 22 | SCL | For BME280, OLED displays |
| **SPI (default)** |
| 23 | MOSI | For SD cards, displays |
| 19 | MISO | For SD cards, displays |
| 18 | SCK | For SD cards, displays |
| 5 | CS | Chip select (avoid during boot) |
| **UART** |
| 1 | TX | Serial output (used for USB) |
| 3 | RX | Serial input (used for USB) |

---

## DHT22 Temperature & Humidity Sensor

**Overview**: Digital temperature and humidity sensor with ±0.5°C accuracy.

### Wiring

```
DHT22 Sensor
┌─────────┐
│    O    │ Pin 1: VCC (3.3V or 5V)
│         │
│  DHT22  │ Pin 2: DATA (to GPIO)
│         │
│    O    │ Pin 3: NC (not connected)
│    O    │ Pin 4: GND
└─────────┘

ESP32 Connection:
┌────────────────────┐
│      ESP32         │
│                    │
│  3V3 ────────────● VCC
│                    │
│  GPIO 4 ──────────● DATA (with 10kΩ pullup to VCC)
│                    │
│  GND ────────────● GND
│                    │
└────────────────────┘
```

### Pullup Resistor

```
        3.3V
         │
        [10kΩ]
         │
GPIO 4 ──┴─── DHT22 DATA
```

### Code Example

```cpp
#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);
    dht.begin();
}

void loop() {
    float temp = dht.readTemperature();
    float humidity = dht.readHumidity();
    
    if (!isnan(temp) && !isnan(humidity)) {
        Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", temp, humidity);
    }
    delay(2000);
}
```

### Parts List

- DHT22 sensor: $3-5
- 10kΩ resistor: $0.10
- Jumper wires: $0.50

---

## DS18B20 Waterproof Temperature Sensor

**Overview**: Waterproof 1-Wire temperature sensor with ±0.5°C accuracy, ideal for outdoor/liquid measurements.

### Wiring

```
DS18B20 (3-wire cable)
┌──────────────┐
│   Sensor     │
└──────┬───────┘
       │
   ┌───┴───┬───────┬
   │ Red   │ Yellow│ Black
   │ VCC   │ DATA  │ GND
   │       │       │
   
ESP32 Connection:
┌────────────────────┐
│      ESP32         │
│                    │
│  3V3 ────────────● Red (VCC)
│                    │
│  GPIO 4 ──────────● Yellow (DATA, with 4.7kΩ pullup to VCC)
│                    │
│  GND ────────────● Black (GND)
│                    │
└────────────────────┘
```

### Pullup Resistor (required for 1-Wire)

```
        3.3V
         │
        [4.7kΩ]
         │
GPIO 4 ──┴─── DS18B20 DATA (Yellow)
```

### Code Example

```cpp
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(115200);
    sensors.begin();
}

void loop() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    
    Serial.printf("Water Temp: %.2f°C\n", temp);
    delay(1000);
}
```

### Parts List

- DS18B20 waterproof probe: $4-6
- 4.7kΩ resistor: $0.10
- Jumper wires: $0.50

---

## PIR Motion Sensor (HC-SR501)

**Overview**: Passive infrared motion detector, triggers HIGH when motion detected.

### Wiring

```
HC-SR501 PIR Sensor
┌───────────────┐
│   ┌─────┐     │
│   │     │     │ Jumper: H (repeatable trigger) / L (single trigger)
│   └─────┘     │
│               │
│  VCC DATA GND │
│   O    O   O  │
└───┼────┼───┼──┘
    │    │   │

ESP32 Connection:
┌────────────────────┐
│      ESP32         │
│                    │
│  5V ─────────────● VCC (needs 5V for reliable detection)
│                    │
│  GPIO 13 ─────────● DATA (output is 3.3V compatible)
│                    │
│  GND ────────────● GND
│                    │
└────────────────────┘
```

### Sensitivity Adjustment

```
HC-SR501 Top View:
┌────────────────┐
│   [Sx]   [Tx]  │  Sx: Sensitivity (distance: 3-7m)
│       ┌────┐   │  Tx: Time delay (0.3-200s)
│       │Lens│   │
│       └────┘   │
└────────────────┘
```

### Code Example

```cpp
#define PIR_PIN 13

void setup() {
    Serial.begin(115200);
    pinMode(PIR_PIN, INPUT);
    delay(2000);  // PIR warmup time
}

void loop() {
    if (digitalRead(PIR_PIN) == HIGH) {
        Serial.println("🚨 Motion detected!");
        delay(500);
    }
}
```

### Parts List

- HC-SR501 PIR sensor: $2-3
- Jumper wires: $0.50

---

## Relay Module (Single Channel)

**Overview**: Control high-voltage devices (AC lights, pumps) safely with 5V signal.

### Wiring

```
Relay Module
┌──────────────────┐
│    ┌────────┐    │
│ IN │        │ COM │  COM: Common (connect to AC live)
│ O  │  Relay │  O  │  NO: Normally Open (connect to device)
│    │        │  O  │  NC: Normally Closed (unused)
│ VCC└────────┘ NO  │
│  O          NC O  │
│ GND          O    │
│  O               │
└──────────────────┘

ESP32 Connection:
┌────────────────────┐
│      ESP32         │
│                    │
│  5V ─────────────● VCC
│                    │
│  GPIO 26 ─────────● IN (active LOW to trigger)
│                    │
│  GND ────────────● GND
│                    │
└────────────────────┘

High-Voltage Side (⚠️ AC mains):
┌─────────────────────────┐
│  AC Power Source        │
│  (110V/220V)            │
│                         │
│  Live ───────● COM      │
│              │          │
│              Relay      │
│              │          │
│  Device ─────● NO       │
│  (Lamp)                 │
│              │          │
│  Neutral ────┴──────────┤
│                         │
└─────────────────────────┘
```

### Safety Warning

⚠️ **HIGH VOLTAGE**: AC mains (110V/220V) can be **FATAL**. 
- Turn OFF power before wiring
- Use proper wire gauge (14-16 AWG for 15A)
- Insulate all connections
- Keep relay module away from water
- If unsure, hire an electrician

### Code Example

```cpp
#define RELAY_PIN 26

void setup() {
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, HIGH);  // OFF (relay is active-LOW)
}

void loop() {
    digitalWrite(RELAY_PIN, LOW);   // ON
    delay(2000);
    digitalWrite(RELAY_PIN, HIGH);  // OFF
    delay(2000);
}
```

### Parts List

- 1-channel 5V relay module: $2-3
- Jumper wires: $0.50
- AC wiring: consult local electrical code

---

## Power Supply Guidelines

### USB Power (Default)

```
USB 5V (500mA max)
   │
   ▼
ESP32 (3.3V regulator)
   │
   ├─ ESP32 core: ~80-160mA (WiFi active)
   ├─ DHT22: ~1-2mA
   ├─ PIR sensor: ~50-60mA
   └─ Relay coil: ~70mA
```

**Total**: ~200-300mA → **USB is sufficient**

### External Power (for Relays/Motors)

```
External 5V (2A+)
   │
   ├─ ESP32 VIN (or 5V pin)
   └─ Relay/Motor (separate power rail)
```

**When to use external power**:
- Multiple relays (>2)
- Motors, servos, solenoids
- High-power sensors (CO2, PM2.5)

### Battery Power

```
LiPo 3.7V (1000-2000mAh)
   │
   ├─ ESP32 3.3V pin (bypass regulator)
   └─ Deep sleep mode: ~10-50µA
```

**Expected runtime**: 1000mAh / 50µA = ~20,000 hours (833 days in deep sleep)

---

## Common Issues

### DHT22 reads NaN

**Causes**:
- Missing pullup resistor (10kΩ required)
- Wrong GPIO pin (use 4, 13, 14, etc.)
- Sensor not powered (check 3.3V)

**Fix**:
```cpp
delay(2000);  // Wait longer between reads
```

### DS18B20 not detected

**Causes**:
- Missing pullup resistor (4.7kΩ required)
- Multiple sensors on same bus (use `sensors.getDeviceCount()`)

**Fix**:
```cpp
sensors.begin();
Serial.printf("Found %d devices\n", sensors.getDeviceCount());
```

### PIR false triggers

**Causes**:
- Too sensitive (adjust Sx potentiometer)
- Facing window/heater (infrared interference)
- Not warmed up (wait 30-60 seconds after power-on)

**Fix**:
- Turn Sx counterclockwise to reduce range
- Shield sensor from direct sunlight

### Relay clicks but device doesn't turn on

**Causes**:
- Wrong terminal (use NO, not NC)
- Loose AC wire connection
- Insufficient GPIO current (add transistor)

**Fix**:
```
ESP32 GPIO 26 ──[1kΩ]── NPN transistor base
                         (2N2222 or BC547)
         5V ────────────┬─ Relay VCC
                        │
Transistor collector ───┘
Transistor emitter ────── GND
```

---

## Next Steps

- **Add sensors**: Install drivers for DHT22, DS18B20, etc.
- **Test wiring**: Use example code to verify connections
- **Deploy**: Integrate sensors into `src/main.cpp`
- **Low power**: Enable deep sleep for battery operation (see issue #5)

For more wiring examples, see the [MicroClaw examples folder](../examples/).

---

**⚠️ Safety First**: Always disconnect power when wiring. Double-check polarity. If unsure, ask for help.
