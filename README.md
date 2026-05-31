# Beacon

Office door LED sign powered by an ESP32 and a P4 HUB75 panel. Flashes animations to mark your office, displays custom messages on demand via a simple HTTP API.

## Stack

| Layer | Tech |
|---|---|
| Display | P4 LED panel — 64×32 dots, 256×128mm, HUB75 |
| Brain | ESP32 (WiFi built in) |
| Firmware | PlatformIO + ESP32-HUB75-MatrixPanel-I2S-DMA + AsyncWebServer |
| Client | Mobile-friendly web app |

## Repo Structure

```
beacon/
├── firmware/   ← ESP32 PlatformIO project
├── client/     ← Web app for sending messages
└── docs/       ← Documentation (Zensical)
```

## Docs

Documentation is served via GitHub Pages.

## Quick Start

See [Getting Started](docs/getting-started/index.md).
