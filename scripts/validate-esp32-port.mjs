import fs from "node:fs";

const requiredFiles = [
  "platformio.ini",
  "src/main.cpp",
  "include/MicroClawConfig.h",
  "lib/DHT22Sensor/DHT22Sensor.h",
  "lib/DHT22Sensor/DHT22Sensor.cpp",
  "lib/MicroClawMqtt/MicroClawMqtt.h",
  "lib/MicroClawMqtt/MicroClawMqtt.cpp",
  "lib/BatteryMonitor/BatteryMonitor.h",
  "lib/BatteryMonitor/BatteryMonitor.cpp",
  "docs/wiring.md",
];

const requiredTerms = [
  ["platformio.ini", "esp32-c3-devkitm-1"],
  ["platformio.ini", "PubSubClient"],
  ["src/main.cpp", "ArduinoOTA"],
  ["src/main.cpp", "esp_deep_sleep_start"],
  ["src/main.cpp", "publishSensor"],
  ["src/main.cpp", "battery_low"],
  ["lib/DHT22Sensor/DHT22Sensor.cpp", "DHT22"],
  ["lib/MicroClawMqtt/MicroClawMqtt.cpp", "clawland/"],
  ["docs/wiring.md", "DHT22"],
  ["docs/wiring.md", "Relay"],
];

const missing = requiredFiles.filter((file) => !fs.existsSync(file));
if (missing.length) {
  throw new Error(`Missing required files: ${missing.join(", ")}`);
}

for (const [file, term] of requiredTerms) {
  const text = fs.readFileSync(file, "utf8");
  if (!text.includes(term)) {
    throw new Error(`${file} does not include required term: ${term}`);
  }
}

console.log("MicroClaw ESP32-C3 port structure validated");
