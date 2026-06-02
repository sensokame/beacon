#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "cfg.h"
#include "display.h"
#include "http_server.h"
#include "serial_cmd.h"
#include <EspOta.h>

static void connectWiFi() {
    WiFi.setHostname(cfgHost.c_str());
    WiFi.begin(cfgSSID.c_str(), cfgPass.c_str());
    Serial.print("# Connecting to WiFi");
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        if (millis() - start > 10000) {
            Serial.println("\n# WiFi timeout — continuing offline");
            return;
        }
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\n# Connected — IP: %s\n", WiFi.localIP().toString().c_str());
    if (MDNS.begin(cfgHost.c_str()))
        Serial.printf("# mDNS: http://%s.local\n", cfgHost.c_str());
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("# Beacon starting");
    loadConfig();
    initDisplay();
    showIdle();
    connectWiFi();
    initServer();
    Serial.println("# Ready");
}

void loop() {
    handleSerial();
    if (messageExpiry > 0 && millis() > messageExpiry) showIdle();
    if (otaRebootPending()) {
        delay(500);
        ESP.restart();
    }
    delay(100);
}
