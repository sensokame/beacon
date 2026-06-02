#include "http_server.h"
#include "display.h"
#include "config.h"
#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <EspOta.h>

static AsyncWebServer server(80);

bool otaRebootPending() { return EspOta::rebootPending(); }

static const char INDEX_HTML[] PROGMEM = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Beacon</title>
<style>
* { box-sizing: border-box; margin: 0; padding: 0; }
body { background: #0d0d0d; color: #e0e0e0; font-family: monospace; padding: 24px; max-width: 480px; margin: 0 auto; }
h1 { color: #00ff50; font-size: 1.8rem; letter-spacing: 4px; margin-bottom: 6px; }
.topbar { display: flex; justify-content: space-between; align-items: flex-start; margin-bottom: 28px; }
.sub { color: #444; font-size: 0.75rem; display: flex; align-items: center; gap: 6px; }
.boot-link { color: #ff9500; font-size: 0.75rem; text-decoration: none; letter-spacing: 1px; white-space: nowrap; padding-top: 2px; }
.boot-link:hover { opacity: 0.75; }
.dot { width: 8px; height: 8px; border-radius: 50%; background: #333; flex-shrink: 0; }
.dot.online { background: #00ff50; }
.card { background: #141414; border: 1px solid #222; border-radius: 6px; padding: 20px; margin-bottom: 14px; }
.card h2 { font-size: 0.65rem; letter-spacing: 3px; color: #444; text-transform: uppercase; margin-bottom: 16px; }
.row { display: flex; justify-content: space-between; align-items: baseline; font-size: 0.85rem; margin-bottom: 8px; }
.row:last-child { margin-bottom: 0; }
.key { color: #444; }
.val { color: #ccc; text-align: right; }
.val.active { color: #00ff50; }
input[type=text], input[type=number] {
  background: #0d0d0d; border: 1px solid #2a2a2a; color: #e0e0e0;
  padding: 10px 12px; border-radius: 4px; font-family: monospace;
  font-size: 0.9rem; width: 100%; margin-bottom: 10px;
}
input:focus { outline: none; border-color: #00ff50; }
.btn-row { display: flex; gap: 8px; }
button {
  padding: 10px 18px; border: none; border-radius: 4px;
  font-family: monospace; font-size: 0.85rem; cursor: pointer; flex: 1;
}
button:disabled { opacity: 0.4; cursor: not-allowed; }
.btn-primary { background: #00ff50; color: #000; font-weight: bold; }
.btn-ghost { background: #1e1e1e; color: #888; border: 1px solid #2a2a2a; }
.btn-ghost:hover:not(:disabled) { color: #ff4444; border-color: #ff4444; }
.btn-primary:hover:not(:disabled) { opacity: 0.85; }
</style>
</head>
<body>

<h1>BEACON</h1>
<div class="topbar">
  <div class="sub">
    <span class="dot" id="dot"></span>
    <span id="conn-label">connecting...</span>
  </div>
  <a href="/ota" class="boot-link">boot mode &#8594;</a>
</div>

<div class="card">
  <h2>Status</h2>
  <div class="row"><span class="key">message</span><span class="val" id="s-msg">&mdash;</span></div>
  <div class="row"><span class="key">uptime</span><span class="val" id="s-uptime">&mdash;</span></div>
  <div class="row"><span class="key">ip</span><span class="val" id="s-ip">&mdash;</span></div>
</div>

<div class="card">
  <h2>Message</h2>
  <input type="text" id="msg-text" placeholder="text to display" maxlength="64"
         onkeydown="if(event.key==='Enter') sendMessage()">
  <input type="number" id="msg-timeout" placeholder="timeout in seconds (0 = permanent)" min="0">
  <div class="btn-row">
    <button class="btn-primary" onclick="sendMessage()">Send</button>
    <button class="btn-ghost" onclick="clearMessage()">Clear</button>
  </div>
</div>

<script>
function fmt(s) {
  if (s < 60) return s + 's';
  if (s < 3600) return Math.floor(s/60) + 'm ' + (s%60) + 's';
  return Math.floor(s/3600) + 'h ' + Math.floor((s%3600)/60) + 'm';
}
async function fetchStatus() {
  try {
    const r = await fetch('/status');
    const d = await r.json();
    document.getElementById('dot').className = 'dot online';
    document.getElementById('conn-label').textContent = 'online';
    const msgEl = document.getElementById('s-msg');
    msgEl.textContent = d.hasMessage ? d.message : '(idle)';
    msgEl.className = 'val' + (d.hasMessage ? ' active' : '');
    document.getElementById('s-uptime').textContent = fmt(d.uptime);
    document.getElementById('s-ip').textContent = d.ip;
  } catch {
    document.getElementById('dot').className = 'dot';
    document.getElementById('conn-label').textContent = 'offline';
  }
}
async function sendMessage() {
  const text = document.getElementById('msg-text').value.trim();
  if (!text) return;
  const timeout = parseInt(document.getElementById('msg-timeout').value) || 0;
  await fetch('/message', {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify({text, timeout})
  });
  fetchStatus();
}
async function clearMessage() {
  await fetch('/message', {method: 'DELETE'});
  fetchStatus();
}
fetchStatus();
setInterval(fetchStatus, 5000);
</script>
</body>
</html>
)html";

void initServer() {
    // POST /message
    server.addHandler(new AsyncCallbackJsonWebHandler("/message",
        [](AsyncWebServerRequest *req, JsonVariant &json) {
            String text = json["text"] | "";
            int timeout = json["timeout"] | 0;
            if (text.isEmpty()) {
                req->send(400, "application/json", "{\"error\":\"text required\"}");
                return;
            }
            currentMessage = text;
            messageExpiry = timeout > 0 ? millis() + (unsigned long)timeout * 1000 : 0;
            showMessage(text.c_str(), displayColor(255, 255, 255));
            req->send(200, "application/json", "{\"ok\":true}");
        }
    ));

    // DELETE /message
    server.on("/message", HTTP_DELETE, [](AsyncWebServerRequest *req) {
        showIdle();
        req->send(200, "application/json", "{\"ok\":true}");
    });

    // GET /status
    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *req) {
        StaticJsonDocument<256> doc;
        doc["message"]    = currentMessage;
        doc["hasMessage"] = !currentMessage.isEmpty();
        doc["ip"]         = WiFi.status() == WL_CONNECTED
                                ? WiFi.localIP().toString()
                                : "offline";
        doc["uptime"]     = millis() / 1000;
        String body;
        serializeJson(doc, body);
        req->send(200, "application/json", body);
    });

    // GET / — redirect to boot mode if active, otherwise serve main UI
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *req) {
        if (EspOta::isBootMode()) { req->redirect("/ota"); return; }
        req->send(200, "text/html", INDEX_HTML);
    });

    EspOta::init(server, strlen(OTA_PASSWORD) > 0 ? OTA_PASSWORD : nullptr);

    server.begin();
    Serial.println("# HTTP server started");
}
