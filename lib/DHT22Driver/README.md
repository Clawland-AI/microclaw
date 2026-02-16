# DHT22Driver Library

Production-ready driver for DHT22 temperature and humidity sensor with error handling and JSON output.

## Features

- **Automatic retry logic** (configurable retries and delay)
- **Range validation** (temperature: -40°C to 80°C, humidity: 0-100%)
- **JSON output** compatible with PicoClaw/NanoClaw reporting
- **Error messages** for debugging
- **Efficient batch reads** (read temperature and humidity together)

## Installation

Already included in MicroClaw! No extra installation needed.

PlatformIO will automatically discover this library in `lib/DHT22Driver/`.

## Wiring

```
DHT22 Sensor
┌─────────┐
│    O    │ Pin 1: VCC (3.3V)
│  DHT22  │ Pin 2: DATA (to GPIO 4 with 10kΩ pullup)
│    O    │ Pin 3: NC (not connected)
│    O    │ Pin 4: GND
└─────────┘

ESP32 Connection:
3.3V ────────────● VCC
                  │
GPIO 4 ──┬────────● DATA
         │
     [10kΩ] (pullup to 3.3V)
         │
GND ─────┴────────● GND
```

**Important**: 10kΩ pullup resistor required on DATA pin.

## Usage

### Basic Example

```cpp
#include <DHT22Driver.h>

// Initialize driver on GPIO 4
DHT22Driver sensor(4);

void setup() {
    Serial.begin(115200);
    sensor.begin();
}

void loop() {
    // Read temperature and humidity
    float temp = sensor.readTemperature();
    float humidity = sensor.readHumidity();
    
    if (sensor.isOK()) {
        Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", temp, humidity);
    } else {
        Serial.println("Error: " + sensor.getError());
    }
    
    delay(5000);
}
```

### JSON Output (for MQTT/HTTP reporting)

```cpp
#include <DHT22Driver.h>

DHT22Driver sensor(4);

void setup() {
    Serial.begin(115200);
    sensor.begin();
}

void loop() {
    String json = sensor.readJSON();
    Serial.println(json);
    // Output: {"temperature":25.0,"humidity":60.0,"status":"ok"}
    
    delay(5000);
}
```

### Efficient Batch Read

```cpp
#include <DHT22Driver.h>

DHT22Driver sensor(4);

void setup() {
    Serial.begin(115200);
    sensor.begin();
}

void loop() {
    float temp, humidity;
    
    if (sensor.read(temp, humidity)) {
        Serial.printf("Temp: %.1f°C, Humidity: %.1f%%\n", temp, humidity);
    } else {
        Serial.println("Error: " + sensor.getError());
    }
    
    delay(5000);
}
```

### Custom Retry Configuration

```cpp
#include <DHT22Driver.h>

// 5 retries with 1-second delay between attempts
DHT22Driver sensor(4, 5, 1000);

void setup() {
    Serial.begin(115200);
    sensor.begin();
}

void loop() {
    String json = sensor.readJSON();
    Serial.println(json);
    
    delay(5000);
}
```

## API Reference

### Constructor

```cpp
DHT22Driver(uint8_t pin, uint8_t retries = 3, uint16_t retryDelay = 2000);
```

- `pin`: GPIO pin connected to DHT22 DATA pin
- `retries`: Number of read attempts on failure (default: 3)
- `retryDelay`: Delay between retries in milliseconds (default: 2000)

### Methods

#### `void begin()`

Initialize the sensor. Call once in `setup()`.

#### `float readTemperature()`

Read temperature in Celsius. Returns `NaN` on error.

#### `float readTemperatureF()`

Read temperature in Fahrenheit. Returns `NaN` on error.

#### `float readHumidity()`

Read relative humidity (%). Returns `NaN` on error.

#### `bool read(float &temp, float &humidity)`

Read both temperature and humidity in one call (more efficient).

Returns `true` on success, `false` on error. Output parameters are set on success.

#### `String readJSON()`

Read temperature and humidity and return as JSON string.

**Success:**
```json
{"temperature":25.0,"humidity":60.0,"status":"ok"}
```

**Error:**
```json
{"error":"Sensor read failed after 3 attempts","status":"error"}
```

#### `bool isOK()`

Check if last read was successful.

#### `String getError()`

Get error message from last failed read. Returns empty string if no error.

## Error Handling

The driver validates:
- **NaN values** from sensor
- **Temperature range**: -40°C to 80°C
- **Humidity range**: 0% to 100%

Error messages:
- `"Sensor read failed (both values NaN)"` — Communication failure
- `"Temperature out of range: XX.XC"` — Invalid reading
- `"Humidity out of range: XX.X%"` — Invalid reading
- `"... after N attempts"` — All retries exhausted

## Troubleshooting

### Sensor always returns NaN

**Causes:**
- Missing 10kΩ pullup resistor on DATA pin
- Wrong GPIO pin
- Loose connection

**Fix:**
```cpp
// Add debug output
sensor.begin();
delay(2000);  // Wait for sensor to stabilize

String json = sensor.readJSON();
Serial.println(json);  // Check error message
```

### Intermittent failures

**Causes:**
- Insufficient delay between reads (sensor needs 2+ seconds)
- Electrical noise on DATA line

**Fix:**
```cpp
// Increase retry delay
DHT22Driver sensor(4, 5, 3000);  // 5 retries, 3s delay
```

### Values out of range

**Causes:**
- Faulty sensor
- Extreme environment

**Fix:**
- Check sensor with multimeter (DATA pin should idle HIGH)
- Replace sensor if consistently fails validation

## Performance

- **Read time**: ~250ms per successful read
- **Memory**: ~200 bytes
- **CPU**: Minimal (blocking I/O)

## Dependencies

- Adafruit DHT sensor library (installed via `platformio.ini`)

## License

Apache License 2.0 (same as MicroClaw)

## Examples

See `examples/dht22_example/` for complete working examples.

---

**Built for MicroClaw Issue #2**
