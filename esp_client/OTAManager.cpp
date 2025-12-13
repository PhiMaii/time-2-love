#include <string>
#include <ESP8266WiFi.h>
#include <otadrive_esp.h>

#include "EEPROMManager.h"
#include "OTAManager.h"
#include "DisplayManager.h"

#include "Config.h"

unsigned long OTAManager::_lastCheck = 0;

// Check every 10 minutes
// static const unsigned long OTA_CHECK_INTERVAL = 10UL * 60UL * 1000UL;

void OTAManager::begin(String deviceID, String SwVersion) {
    Serial.println("[OTA] Initializing OTAdrive");

    OTADRIVE.setInfo(OTA_DRIVE_API_KEY, SwVersion);
    // OTADRIVE.onUpdateFirmwareProgress(onUpdateProgress);
    OTADRIVE.onUpdateFirmwareProgress(DisplayManager::displayUpdateProgress);

    Serial.println("[OTA] Device ID: " + deviceID);
    Serial.println("[OTA] Firmware Version: " + SwVersion);
}

bool OTAManager::sendAlive(){
  if(DEBUG_MODE) Serial.println(String(OTA_PREFIX) + "Sending alive-packet");
  // return OTADRIVE.sendAlive();
  return true;
}

void OTAManager::checkForUpdate() {
    if (WiFi.status() != WL_CONNECTED) return;

    Serial.println(String(OTA_PREFIX) + "Checking for firmware update...");

    auto inf = OTADRIVE.updateFirmwareInfo();

    Serial.println(inf);

    if(inf.available){
      Serial.printf("Updating to: %s", inf.version.c_str());

      OTADRIVE.updateFirmware(false);

      Serial.println(String(OTA_PREFIX) + "Update successful!");
      Serial.println(String(OTA_PREFIX) + "Writing new version to EEPROM ...");

      EEPROMManager::setSwVersion(inf.version.c_str());

      ESP.restart();
    }
}