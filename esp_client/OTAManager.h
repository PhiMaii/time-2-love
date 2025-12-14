#pragma once
#include <Arduino.h>

class OTAManager {
public:
    OTAManager();
    static void begin(String deviceID, String swVersion);
    static bool sendAlive();
    static void checkForUpdate();
    static void downloadUpdate();
    static const char* getNewSwVersion();  // static + const char*

private:
    static String _newSwVersion;     // Add static!
    static unsigned long _lastCheck; // Add static!
};