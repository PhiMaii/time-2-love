#include "EEPROMManager.h"

static String _deviceId;
static String _swVersion;

void EEPROMManager::begin() {
  EEPROM.begin(EEPROM_SIZE);
}

void EEPROMManager::loadOrInitialize() {
  uint8_t magic = EEPROM.read(EEPROM_MAGIC_ADDR);

  if (magic != EEPROM_MAGIC_VAL) {
    // EEPROM is uninitialized → write defaults
    Serial.println("[EEPROM] Writing default values...");

    EEPROM.write(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VAL);

    writeString(EEPROM_DEVICE_ID_ADDR, EEPROM_DEVICE_ID_LEN, "time2love-99");
    writeString(EEPROM_SW_VERSION_ADDR, EEPROM_SW_VERSION_LEN, "0.0.0");

    EEPROM.commit();
  }

  // Load values
  _deviceId = readString(EEPROM_DEVICE_ID_ADDR, EEPROM_DEVICE_ID_LEN);
  _swVersion = readString(EEPROM_SW_VERSION_ADDR, EEPROM_SW_VERSION_LEN);

  // Serial.println("[EEPROM] Loaded:");
  // Serial.println("  Device ID: " + _deviceId);
  // Serial.println("  SW Version: " + _swVersion);
}

String EEPROMManager::getDeviceId() {
  return _deviceId;
}
String EEPROMManager::getSwVersion() {
  return _swVersion;
}

String EEPROMManager::readString(int addr, int maxLen) {
  char buf[maxLen + 1];
  for (int i = 0; i < maxLen; i++) {
    buf[i] = EEPROM.read(addr + i);
  }
  buf[maxLen] = '\0';
  return String(buf);
}

void EEPROMManager::writeString(int addr, int maxLen, const String& s) {
  for (int i = 0; i < maxLen; i++) {
    char c = (i < s.length()) ? s[i] : 0;
    EEPROM.write(addr + i, c);
  }
}

void EEPROMManager::setSwVersion(const String& version) {
  writeString(EEPROM_SW_VERSION_ADDR, EEPROM_SW_VERSION_LEN, version.c_str());
}