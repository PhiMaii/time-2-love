// OTAManager.h

#ifndef OTAMANAGER_H
#define OTAMANAGER_H

#include <Arduino.h>

class OTAManager {
public:
    OTAManager();
    static void begin(String deviceID, String swVersion);
    static bool sendAlive();
    static bool checkForUpdate();
    static void downloadUpdate();
    static const char* getNewSwVersion();  // static + const char*
    static const char* getCurrentSwVersion();

private:
    static String _newSwVersion;     // Add static!
    static String _currentSwVersion;
    static unsigned long _lastCheck; // Add static!
};

#endif