# Client

The web client is a mobile-friendly single-page app for sending messages and triggering animations without needing to `curl` from a terminal.

## Features

- Text input + send button
- Quick-access buttons for common messages (e.g. "In a meeting", "Back soon", "Do not disturb")
- Animation trigger buttons
- Shows current display state

## Usage

Open the client in any browser on the same network as the beacon:

```
http://<beacon-ip>/
```

The ESP32 serves the client directly — no separate server needed.

Alternatively, host it anywhere and point it at the beacon's IP via a config field.

## Development

```bash
cd client
npm install
npm run dev
```

Build for production (output goes into `firmware/data/` for SPIFFS upload):

```bash
npm run build
```
