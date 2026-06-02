#include "cfg.h"
#include "config.h"
#include <Preferences.h>

String cfgSSID;
String cfgPass;
String cfgHost;

static Preferences prefs;

void loadConfig() {
    prefs.begin("beacon", true);
    cfgSSID = prefs.getString("ssid", WIFI_SSID);
    cfgPass = prefs.getString("pass", WIFI_PASSWORD);
    cfgHost = prefs.getString("host", WIFI_HOSTNAME);
    prefs.end();
}

void saveConfig() {
    prefs.begin("beacon", false);
    prefs.putString("ssid", cfgSSID);
    prefs.putString("pass", cfgPass);
    prefs.putString("host", cfgHost);
    prefs.end();
}
