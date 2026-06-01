# MicroClaw MQTT Telemetry Contract

This contract documents the retained MQTT messages emitted by the ESP32-C3 and ESP32 PlatformIO environments.

## Topic Tree

| Topic | Retained | Purpose |
| --- | --- | --- |
| `clawland/{node_id}/status` | yes | Wake/sleep lifecycle and firmware status. |
| `clawland/{node_id}/sensors/dht22` | yes | Latest DHT22 temperature and humidity reading. |
| `clawland/{node_id}/alerts/warning` | yes | Recoverable sensor or runtime warnings. |
| `clawland/{node_id}/alerts/critical` | yes | Battery or deployment-critical alerts. |

`node_id` comes from `MICROCLAW_NODE_ID`, so each deployed board publishes to a stable namespace.

## Status Payload

```json
{
  "node_id": "microclaw-esp32c3-001",
  "state": "awake",
  "battery_mv": 3812,
  "sleep_seconds": 300,
  "ota_window_seconds": 20,
  "firmware": "0.2.0"
}
```

Valid `state` values are `awake` and `sleeping`.

## DHT22 Sensor Payload

```json
{
  "node_id": "microclaw-esp32c3-001",
  "sensor_type": "dht22",
  "temperature_c": 24.1,
  "humidity_percent": 61.4,
  "battery_mv": 3812,
  "firmware": "0.2.0"
}
```

## Alert Payload

```json
{
  "node_id": "microclaw-esp32c3-001",
  "code": "battery_low",
  "battery_mv": 3290
}
```

Known alert codes include `battery_low` and DHT22 read failure codes returned by the sensor wrapper.

## Wake Cycle

1. Wake from timer or reset.
2. Read battery and DHT22.
3. Connect Wi-Fi and MQTT.
4. Publish `status` with `state=awake`.
5. Publish sensor or warning payload.
6. Publish critical low-battery alert when needed.
7. Keep ArduinoOTA open for the configured maintenance window.
8. Publish `status` with `state=sleeping`.
9. Enter deep sleep for `MICROCLAW_SLEEP_SECONDS`.

## Validation Notes

The local validation script checks that both `esp32c3` and `esp32` PlatformIO environments are present, the telemetry contract is documented, and the firmware publishes status, sensor, and alert messages. Hardware behavior is not marked hardware-tested until the PR is run on a physical board.
