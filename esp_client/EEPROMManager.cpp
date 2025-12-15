// EEPROMManager.cpp

#include "EEPROMManager.h"
#include "Config.h"

EepromData EEPROMManager::_data;

void EEPROMManager::begin() {
  EEPROM.begin(EEPROM_SIZE);
}

void EEPROMManager::loadOrInitialize() {
  // uint8_t magic = EEPROM.read(EEPROM_MAGIC_ADDR);
  EEPROM.get(EEPROM_MAGIC_ADDR, _data);  // Faster read

  if (_data.magic != EEPROM_MAGIC_VAL) {
    Serial.println("[EEPROM] Writing default values...");

    _data.magic = EEPROM_MAGIC_VAL;
    strcpy(_data.deviceId, "time2love-01");
    strcpy(_data.swVersion, "0.1.2");
    strcpy(_data.WiFiSSID, "YourWifiSSIDHere");
    strcpy(_data.WiFiPassword, "YourWifiPasswordHere");

    EEPROM.put(EEPROM_MAGIC_ADDR, _data);
    EEPROM.commit();
  }
  Serial.println("####################\n[EEPROM] Loaded:");
  Serial.printf("  - Device ID: %s\n", _data.deviceId);
  Serial.printf("  - SW Version: %s\n", _data.swVersion);
  Serial.printf("  - SSID: %s\n", _data.WiFiSSID);
  Serial.printf("  - WiFi Password: %s\n####################\n", _data.WiFiPassword);
}

String EEPROMManager::getDeviceId() {
  return String(_data.deviceId);
}

String EEPROMManager::getSwVersion() {
  return String(_data.swVersion);
}

String EEPROMManager::getSsid() {
  return String(_data.WiFiSSID);
}

String EEPROMManager::getWifiPassword() {
  return String(_data.WiFiPassword);
}

void EEPROMManager::setSwVersion(const String& version) {
    Serial.println(String(EEPROM_PREFIX) + "Writing SW version...");
    strncpy(_data.swVersion, version.c_str(), EEPROM_SW_VERSION_LEN - 1);
    _data.swVersion[EEPROM_SW_VERSION_LEN - 1] = '\0';
    
    EEPROM.put(EEPROM_MAGIC_ADDR, _data);
    EEPROM.commit();
    Serial.println(String(EEPROM_PREFIX) + "Success!");
}

void EEPROMManager::setSsid(const String& ssid) {
    strncpy(_data.WiFiSSID, ssid.c_str(), EEPROM_SSID_LEN - 1);
    _data.WiFiSSID[EEPROM_SSID_LEN - 1] = '\0';
    
    EEPROM.put(EEPROM_MAGIC_ADDR, _data);
    EEPROM.commit();
}

void EEPROMManager::setWiFiPassword(const String& password) {
    strncpy(_data.WiFiPassword, password.c_str(), EEPROM_WIFI_PASSWORD_LEN - 1);
    _data.WiFiPassword[EEPROM_WIFI_PASSWORD_LEN - 1] = '\0';
    
    EEPROM.put(EEPROM_MAGIC_ADDR, _data);
    EEPROM.commit();
}