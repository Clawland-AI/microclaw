# MicroClaw ESP32-C3 Wiring Guide

This guide documents the default wiring used by the ESP32-C3 PlatformIO port. Pins can be changed through `platformio.ini` build flags.

## Default Pin Reference

| Signal | ESP32-C3 GPIO | Notes |
| --- | --- | --- |
| DHT22 data | GPIO4 | Add a 10 kOhm pull-up from DATA to 3V3. |
| Battery sense | GPIO0 ADC | Use a 2:1 divider for single-cell LiPo voltage. |
| DS18B20 data | GPIO5 | Add a 4.7 kOhm pull-up from DATA to 3V3. |
| PIR output | GPIO6 | Use 3V3-compatible PIR modules. |
| Relay control | GPIO7 | Drive through a transistor or opto-isolated relay board. |
| MQTT/Wi-Fi | onboard radio | Configure SSID and broker in `platformio.ini`. |

## DHT22 Temperature/Humidity

```text
ESP32-C3 3V3  -> DHT22 VCC
ESP32-C3 GND  -> DHT22 GND
ESP32-C3 GPIO4 -> DHT22 DATA
3V3 -- 10 kOhm -- DATA
```

## DS18B20 Waterproof Temperature Probe

```text
ESP32-C3 3V3  -> DS18B20 VDD
ESP32-C3 GND  -> DS18B20 GND
ESP32-C3 GPIO5 -> DS18B20 DATA
3V3 -- 4.7 kOhm -- DATA
```

## PIR Motion Sensor

```text
ESP32-C3 3V3  -> PIR VCC
ESP32-C3 GND  -> PIR GND
ESP32-C3 GPIO6 <- PIR OUT
```

## Relay Module

```text
ESP32-C3 GPIO7 -> relay IN
ESP32-C3 GND   -> relay GND
Relay VCC      -> external 5V or module-rated supply
```

Do not power pumps, heaters, or aerators directly from the ESP32-C3 board. Use a rated relay or MOSFET module and isolate high-voltage loads.
