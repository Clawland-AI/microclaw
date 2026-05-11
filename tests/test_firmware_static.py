import pathlib
import re
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
MAIN = (ROOT / "src" / "main.cpp").read_text(encoding="utf-8")
PLATFORMIO = (ROOT / "platformio.ini").read_text(encoding="utf-8")
README = (ROOT / "README.md").read_text(encoding="utf-8")


class FirmwareStaticTests(unittest.TestCase):
    def test_esp32_dht22_driver_and_thresholds_exist(self):
        self.assertIn("#include <DHT.h>", MAIN)
        self.assertRegex(MAIN, r"DHT\s+dht\(")
        self.assertIn("temperature_c", MAIN)
        self.assertIn("humidity_pct", MAIN)
        self.assertIn("TEMP_ALERT_C", MAIN)
        self.assertIn("HUMIDITY_ALERT_PCT", MAIN)

    def test_mqtt_reporting_and_command_callback_exist(self):
        self.assertIn("#include <PubSubClient.h>", MAIN)
        self.assertIn("PubSubClient mqtt", MAIN)
        self.assertIn("publishTelemetry", MAIN)
        self.assertIn("handleCommand", MAIN)
        self.assertIn("microclaw/", MAIN)
        self.assertIn("/telemetry", MAIN)
        self.assertIn("/command", MAIN)

    def test_deep_sleep_and_wake_interval_are_configured(self):
        self.assertIn("esp_sleep_enable_timer_wakeup", MAIN)
        self.assertIn("esp_deep_sleep_start", MAIN)
        self.assertIn("WAKE_INTERVAL_SECONDS", MAIN)

    def test_platformio_declares_required_libraries(self):
        self.assertIn("knolleary/PubSubClient", PLATFORMIO)
        self.assertIn("adafruit/DHT sensor library", PLATFORMIO)
        self.assertIn("bblanchon/ArduinoJson", PLATFORMIO)

    def test_readme_documents_configuration_and_topics(self):
        self.assertIn("MicroClaw ESP32 Sensor Agent", README)
        self.assertIn("MQTT Topics", README)
        self.assertIn("Deep Sleep", README)
        self.assertIn("DHT22", README)


if __name__ == "__main__":
    unittest.main()
