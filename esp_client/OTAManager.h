#pragma once
#include <Arduino.h>

class OTAManager {
public:
    static void begin(String deviceID, String swVersion);
    static bool sendAlive();
    static void checkForUpdate();

private:
    static unsigned long _lastCheck;
};