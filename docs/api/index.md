# API Reference

The ESP32 exposes a simple HTTP REST API on port 80.

## Endpoints

### `POST /message`

Update the displayed text.

**Request body:**

```json
{
  "text": "In a meeting",
  "timeout": 3600
}
```

| Field | Type | Required | Description |
|---|---|---|---|
| `text` | string | yes | Text to display |
| `timeout` | integer | no | Seconds before returning to idle (0 = stay until next update) |

**Response:**

```json
{ "ok": true }
```

---

### `POST /animation`

Trigger a named animation.

**Request body:**

```json
{ "name": "flash" }
```

Available animations: `flash`, `rainbow`, `pulse`, `scroll`

---

### `DELETE /message`

Clear the current message and return to idle animation.

---

### `GET /status`

Returns the current display state.

**Response:**

```json
{
  "mode": "message",
  "text": "In a meeting",
  "uptime": 3847,
  "ip": "192.168.1.42"
}
```

| Field | Values |
|---|---|
| `mode` | `idle` \| `message` \| `animation` |
