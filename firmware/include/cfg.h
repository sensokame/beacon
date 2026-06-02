#pragma once
#include <Arduino.h>

extern String cfgSSID;
extern String cfgPass;
extern String cfgHost;

void loadConfig();
void saveConfig();
