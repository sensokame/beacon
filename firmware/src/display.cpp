#include "display.h"
#include "config.h"

#ifdef PANEL_ENABLED
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>
static MatrixPanel_I2S_DMA *display = nullptr;
#endif

String currentMessage;
unsigned long messageExpiry = 0;

void initDisplay() {
#ifdef PANEL_ENABLED
    HUB75_I2S_CFG cfg(PANEL_WIDTH, PANEL_HEIGHT, PANELS_NUMBER);
    display = new MatrixPanel_I2S_DMA(cfg);
    if (!display || !display->begin()) {
        Serial.println("# Display init failed — running headless");
        delete display;
        display = nullptr;
        return;
    }
    display->setBrightness8(90);
    display->clearScreen();
    Serial.println("# Display ready");
#else
    Serial.println("# Display disabled (PANEL_ENABLED not set)");
#endif
}

void showMessage(const char *msg, uint16_t color) {
#ifdef PANEL_ENABLED
    if (!display) return;
    display->clearScreen();
    display->setCursor(0, 12);
    display->setTextSize(1);
    display->setTextColor(color);
    display->print(msg);
#endif
}

void showIdle() {
    currentMessage = "";
    messageExpiry = 0;
#ifdef PANEL_ENABLED
    if (!display) return;
    showMessage("BEACON", display->color565(0, 255, 80));
#endif
}

uint16_t displayColor(uint8_t r, uint8_t g, uint8_t b) {
#ifdef PANEL_ENABLED
    if (display) return display->color565(r, g, b);
#endif
    return 0;
}

void setDisplayBrightness(uint8_t brightness) {
#ifdef PANEL_ENABLED
    if (!display) return;
    display->setBrightness8(brightness);
#endif
}

bool isDisplayReady() {
#ifdef PANEL_ENABLED
    return display != nullptr;
#else
    return false;
#endif
}
