#include "wl_definitions.h"
#include <string>
#include <ESP8266WiFi.h>
#include <otadrive_esp.h>

#include "EEPROMManager.h"
#include "OTAManager.h"
#include "DisplayManager.h"

#include "Config.h"

// Static member definitions (REQUIRED!)
String OTAManager::_newSwVersion = "null";
unsigned long OTAManager::_lastCheck = 0;

OTAManager::OTAManager() {
  // Constructor body empty - statics initialized above
}

void OTAManager::begin(String deviceID, String SwVersion) {
  Serial.println("[OTA] Initializing OTAdrive");

  OTADRIVE.setInfo(OTA_DRIVE_API_KEY, SwVersion);
  // OTADRIVE.onUpdateFirmwareProgress(onUpdateProgress);
  // OTADRIVE.onUpdateFirmwareProgress(DisplayManager::displayUpdateProgress);

  Serial.println("[OTA] Device ID: " + deviceID);
  Serial.println("[OTA] Device firmware version: " + SwVersion);
}

// Non-static function with RETURN statement
const char* OTAManager::getNewSwVersion() {  // NO static here!
  return OTAManager::_newSwVersion.c_str();  // Fully qualified name
}

bool OTAManager::sendAlive() {
  if (DEBUG_MODE) Serial.println(String(OTA_PREFIX) + "Sending alive-packet");
  return OTADRIVE.sendAlive();
}

void OTAManager::checkForUpdate() {
  if (WiFi.status() != WL_CONNECTED) return;

  Serial.println(String(OTA_PREFIX) + "Checking for newer Version ...");

  auto inf = OTADRIVE.updateFirmwareInfo();

  if (inf.available) {
    _newSwVersion = inf.version.c_str();
    Serial.printf("[OTA] Found newer Version: %s", _newSwVersion);
  } else Serial.println(String(OTA_PREFIX) + "Current version is up to date!");
}

void OTAManager::downloadUpdate() {
  if (WiFi.status() != WL_CONNECTED) return;

  OTADRIVE.updateFirmware(false);

  Serial.println(String(OTA_PREFIX) + "Update successful!");
  Serial.println(String(OTA_PREFIX) + "Writing new version to EEPROM ...");

  // EEPROMManager::setSwVersion(_newSwVersion);

  ESP.restart();
}