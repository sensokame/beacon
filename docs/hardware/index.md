# Hardware

## Panel Specs

| Property | Value |
|---|---|
| Pitch | P4 (4mm between pixels) |
| Resolution | 64×32 dots |
| Physical size | 256×128mm |
| Scan rate | 1/16 |
| LED type | SMD |
| Interface | HUB75 |
| Power | 5V, up to ~4A at full brightness |

## Power Supply

The panel and ESP32 share a single **5V 4A** supply.

- Panel power connects to the HUB75 power pins (or dedicated connector on the panel)
- ESP32 powered from the same 5V rail via its `VIN` pin (or a small AMS1117 3.3V regulator if needed)

Do not power the panel from USB — it cannot supply enough current at full brightness.

## ESP32 Pin Map

Default pin assignment for `ESP32-HUB75-MatrixPanel-I2S-DMA`:

| HUB75 Signal | ESP32 GPIO |
|---|---|
| R1 | 25 |
| G1 | 26 |
| B1 | 27 |
| R2 | 14 |
| G2 | 12 |
| B2 | 13 |
| A | 23 |
| B | 19 |
| C | 5 |
| D | 17 |
| CLK | 16 |
| LAT | 4 |
| OE | 15 |

> These are the library defaults. Adjust in firmware if your wiring differs.
