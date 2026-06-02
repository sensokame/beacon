#pragma once
#include <Arduino.h>

extern String currentMessage;
extern unsigned long messageExpiry;

void initDisplay();
void showMessage(const char *msg, uint16_t color);
void showIdle();
uint16_t displayColor(uint8_t r, uint8_t g, uint8_t b);
void setDisplayBrightness(uint8_t brightness);
bool isDisplayReady();
