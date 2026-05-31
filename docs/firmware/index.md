# Firmware

## Prerequisites

- [PlatformIO](https://platformio.org/) (VS Code extension or CLI)

## Setup

1. Open `firmware/` in VS Code with PlatformIO installed, or use the CLI:

```bash
cd firmware
pio run
```

2. Copy `src/config.h.example` to `src/config.h` and fill in your WiFi credentials:

```cpp
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"
```

## Flash

```bash
cd firmware
pio run --target upload
```

## Libraries Used

| Library | Purpose |
|---|---|
| `ESP32-HUB75-MatrixPanel-I2S-DMA` | Drive the HUB75 LED panel |
| `ESPAsyncWebServer` | HTTP server for the REST API |
| `ArduinoJson` | Parse/serialize JSON payloads |

## Behavior

- On boot: connects to WiFi, starts HTTP server, plays idle animation
- On `POST /message`: displays the provided text, returns to idle after a configurable timeout
- On `POST /animation`: triggers a named animation immediately
