import fs from 'node:fs';

const requiredFiles = [
  'platformio.ini',
  'src/main.cpp',
  'docs/esp32-c3-port.md',
];

const requiredTerms = [
  ['platformio.ini', '[env:esp32c3]'],
  ['platformio.ini', '[env:esp32]'],
  ['platformio.ini', 'esp32-c3-devkitm-1'],
  ['platformio.ini', 'MICROCLAW_ENABLE_OTA=0'],
  ['platformio.ini', 'MICROCLAW_ENABLE_DEEP_SLEEP=0'],
  ['src/main.cpp', 'ArduinoOTA'],
  ['src/main.cpp', 'PubSubClient'],
  ['src/main.cpp', 'esp_deep_sleep_start'],
  ['src/main.cpp', 'batteryMv'],
  ['src/main.cpp', 'dhtOk'],
  ['src/main.cpp', 'MICROCLAW_OTA_PASSWORD'],
  ['docs/esp32-c3-port.md', 'pio run -e esp32c3'],
  ['docs/esp32-c3-port.md', 'pio run -e esp32'],
];

const missing = requiredFiles.filter((file) => !fs.existsSync(file));
if (missing.length) {
  throw new Error(`Missing required files: ${missing.join(', ')}`);
}

for (const [file, term] of requiredTerms) {
  const text = fs.readFileSync(file, 'utf8');
  if (!text.includes(term)) {
    throw new Error(`${file} does not include required term: ${term}`);
  }
}

console.log('MicroClaw ESP32-C3 port structure validated');
