#pragma once
#include <EEPROM.h>
#include <Arduino.h>

#define EEPROM_SIZE       256
#define EEPROM_MAGIC_ADDR 0
#define EEPROM_MAGIC_VAL  0x42

#define EEPROM_DEVICE_ID_ADDR 1
#define EEPROM_DEVICE_ID_LEN  32

#define EEPROM_SW_VERSION_ADDR 33
#define EEPROM_SW_VERSION_LEN   32

class EEPROMManager {
public:
  static void begin();
  static void loadOrInitialize();
  static String getDeviceId();
  static String getSwVersion();

private:
  static String readString(int addr, int maxLen);
  static void writeString(int addr, int maxLen, const String& s);
};