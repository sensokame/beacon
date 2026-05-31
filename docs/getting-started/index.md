# Getting Started

## What You Need

- ESP32 development board
- P4 HUB75 LED panel (64×32 dots)
- 5V 4A power supply
- HUB75 ribbon cable
- Jumper wires

## Steps

1. **Wire the panel** — connect HUB75 ribbon cable to ESP32 GPIO pins (see [Hardware](../hardware/index.md) for pin map)
2. **Connect power** — power the panel and ESP32 from the same 5V 4A supply
3. **Flash the firmware** — see [Firmware](../firmware/index.md)
4. **Configure WiFi** — set your SSID and password in `firmware/src/config.h`
5. **Send a message** — open the [web client](../client/index.md) or POST to `/message`

## First Test

Once flashed and connected to WiFi, the panel will show an idle animation.

Send a test message:

```bash
curl -X POST http://<beacon-ip>/message \
  -H "Content-Type: application/json" \
  -d '{"text": "Hello!"}'
```
