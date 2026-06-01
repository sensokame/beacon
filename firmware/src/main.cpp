#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "config.h"

MatrixPanel_I2S_DMA *display = nullptr;
AsyncWebServer server(80);

static String currentMessage;
static unsigned long messageExpiry = 0; // millis() deadline; 0 = no timeout

// ── Display ───────────────────────────────────────────────────────────────────

void initDisplay() {
    HUB75_I2S_CFG cfg(PANEL_WIDTH, PANEL_HEIGHT, PANELS_NUMBER);
    display = new MatrixPanel_I2S_DMA(cfg);
    display->begin();
    display->setBrightness8(90);
    display->clearScreen();
}

void showMessage(const char *msg, uint16_t color) {
    display->clearScreen();
    display->setCursor(0, 12);
    display->setTextSize(1);
    display->setTextColor(color);
    display->print(msg);
}

void showIdle() {
    currentMessage = "";
    messageExpiry = 0;
    showMessage("BEACON", display->color565(0, 255, 80));
}

// ── WiFi ──────────────────────────────────────────────────────────────────────

void connectWiFi() {
    WiFi.setHostname(WIFI_HOSTNAME);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > 10000) {
            Serial.println("\nWiFi timeout — continuing offline");
            return;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nConnected — IP: %s\n", WiFi.localIP().toString().c_str());
    if (MDNS.begin(WIFI_HOSTNAME))
        Serial.printf("mDNS: http://%s.local\n", WIFI_HOSTNAME);
}

// ── HTTP server ───────────────────────────────────────────────────────────────

void initServer() {
    // POST /message  {"text": "Hello", "timeout": 60}
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
            showMessage(text.c_str(), display->color565(255, 255, 255));
            req->send(200, "application/json", "{\"ok\":true}");
        }
    ));

    // DELETE /message — clear and return to idle
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

    server.begin();
    Serial.println("HTTP server started on port 80");
}

// ── Entry points ──────────────────────────────────────────────────────────────

void setup() {
    Serial.begin(115200);
    initDisplay();
    showIdle();
    connectWiFi();
    initServer();
}

void loop() {
    if (messageExpiry > 0 && millis() > messageExpiry) showIdle();
    // animations go here (BEA-3)
    delay(100);
}
