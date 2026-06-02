#include "serial_cmd.h"
#include "display.h"
#include "cfg.h"
#include <Arduino.h>
#include <WiFi.h>

static String serialBuf;

static void dispatchCommand(const String &line) {
    String upper = line;
    upper.toUpperCase();

    if (upper.startsWith("MSG ")) {
        String rest = line.substring(4);
        int sp = rest.lastIndexOf(' ');
        int timeout = 0;
        String text = rest;
        if (sp > 0) {
            String last = rest.substring(sp + 1);
            bool isNum = true;
            for (char c : last) if (!isDigit(c)) { isNum = false; break; }
            if (isNum) {
                timeout = last.toInt();
                text = rest.substring(0, sp);
            }
        }
        currentMessage = text;
        messageExpiry = timeout > 0 ? millis() + (unsigned long)timeout * 1000 : 0;
        showMessage(text.c_str(), displayColor(255, 255, 255));
        Serial.println("OK");

    } else if (upper == "CLEAR") {
        showIdle();
        Serial.println("OK");

    } else if (upper == "STATUS") {
        Serial.println("OK:");
        Serial.printf("  message  : %s\n", currentMessage.isEmpty() ? "(none)" : currentMessage.c_str());
        Serial.printf("  uptime   : %lus\n", millis() / 1000);
        Serial.printf("  panel    : %s\n",
#ifdef PANEL_ENABLED
            isDisplayReady() ? "ready" : "init failed"
#else
            "disabled"
#endif
        );
        if (WiFi.status() == WL_CONNECTED) {
            Serial.printf("  ip       : %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("  host     : %s.local\n", cfgHost.c_str());
            Serial.printf("  ssid     : %s\n", cfgSSID.c_str());
            Serial.printf("  rssi     : %d dBm\n", WiFi.RSSI());
        } else {
            Serial.println("  wifi     : offline");
        }

    } else if (upper == "WIFI") {
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("OK:");
            Serial.printf("  ip    : %s\n", WiFi.localIP().toString().c_str());
            Serial.printf("  host  : %s.local\n", cfgHost.c_str());
            Serial.printf("  ssid  : %s\n", cfgSSID.c_str());
            Serial.printf("  rssi  : %d dBm\n", WiFi.RSSI());
            Serial.printf("  mac   : %s\n", WiFi.macAddress().c_str());
        } else {
            Serial.println("OK: offline");
        }

    } else if (upper.startsWith("BRIGHTNESS ")) {
        int val = constrain(line.substring(11).toInt(), 0, 255);
        setDisplayBrightness(val);
        Serial.printf("OK: brightness set to %d\n", val);

    } else if (upper.startsWith("SET SSID ")) {
        cfgSSID = line.substring(9);
        Serial.println("OK: SSID set (SAVE to persist, REBOOT to apply)");

    } else if (upper.startsWith("SET PASS ")) {
        cfgPass = line.substring(9);
        Serial.println("OK: PASS set (SAVE to persist, REBOOT to apply)");

    } else if (upper.startsWith("SET HOST ")) {
        cfgHost = line.substring(9);
        Serial.println("OK: HOST set (SAVE to persist, REBOOT to apply)");

    } else if (upper == "SAVE") {
        saveConfig();
        Serial.println("OK: config saved to NVS");

    } else if (upper == "REBOOT") {
        Serial.println("OK: rebooting");
        delay(100);
        ESP.restart();

    } else if (upper == "HELP") {
        Serial.println("OK:");
        Serial.println("  MSG <text> [seconds]   — show message on panel");
        Serial.println("  CLEAR                  — return to idle");
        Serial.println("  STATUS                 — full device status");
        Serial.println("  WIFI                   — WiFi details");
        Serial.println("  BRIGHTNESS <0-255>     — set panel brightness");
        Serial.println("  SET SSID <value>       — set WiFi SSID");
        Serial.println("  SET PASS <value>       — set WiFi password");
        Serial.println("  SET HOST <value>       — set hostname");
        Serial.println("  SAVE                   — persist config to NVS");
        Serial.println("  REBOOT                 — restart device");

    } else if (upper.length() > 0) {
        Serial.println("ERR: unknown command (try HELP)");
    }
}

void handleSerial() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n' || c == '\r') {
            serialBuf.trim();
            if (serialBuf.length() > 0) {
                dispatchCommand(serialBuf);
                serialBuf = "";
            }
        } else {
            serialBuf += c;
        }
    }
}
