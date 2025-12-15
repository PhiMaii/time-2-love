// EEPROMManager.h

#ifndef EEPROMMANAGER_H
#define EEPROMMANAGER_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Config.h"

struct EepromData {
  uint8_t magic = 0x42;
  char deviceId[EEPROM_DEVICE_ID_LEN];
  char swVersion[EEPROM_SW_VERSION_LEN];
  char WiFiSSID[EEPROM_SSID_LEN];
  char WiFiPassword[EEPROM_WIFI_PASSWORD_LEN];
};


class EEPROMManager {
public:
  static void begin();
  static void loadOrInitialize();
  static String getDeviceId();
  static String getSwVersion();
  static void setSwVersion(const String& version);

private:
  static String readString(int addr, int maxLen);
  static void writeString(int addr, int maxLen, const String& s);
};


#endif