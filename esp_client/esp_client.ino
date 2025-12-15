// esp_client.ino

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>

#include "Config.h"
#include "ServerClient.h"
#include "EventClock.h"
#include "DisplayManager.h"
#include "ButtonHandler.h"
#include "EEPROMManager.h"
#include "OTAManager.h"
#include "WiFiManager.h"

// Globals
ServerClient server;
EventClock eventClock;
DisplayManager displayManager;
ButtonHandler button(BUTTON_PIN);
EEPROMManager eepromManager;
OTAManager otaManager;
WiFiManager wifiManager;

// Timing variables
unsigned long lastEventFetch = 0;
unsigned long lastBlinkPoll = 0;
unsigned long lastRegister = 0;
unsigned long lastOTACheck = 0;

String DEVICE_ID_FROM_EEPROM = "";
String SW_VERSION_FROM_EEPROM = "";

void setup() {
  delay(100);

  Serial.begin(115200);

  delay(500);

  eepromManager.begin();
  eepromManager.loadOrInitialize();

  DEVICE_ID_FROM_EEPROM = eepromManager.getDeviceId();
  SW_VERSION_FROM_EEPROM = eepromManager.getSwVersion();

  Serial.println();
  Serial.print("Starting device " + DEVICE_ID_FROM_EEPROM + "\n");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // builtin LED active LOW on many boards

  displayManager.begin();
  displayManager.showTempMessage("Booting...");

  // WiFi
  wifiManager.begin();
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  Serial.println("NTP configured");
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  Serial.println("NTP configured");

  server.begin(SERVER_URL);
  server.registerDevice(String(DEVICE_ID_FROM_EEPROM));

  eventClock.fetchEventFromServer();

  lastEventFetch = millis();
  lastBlinkPoll = millis();
  lastRegister = millis();
  lastOTACheck = millis();

  button.begin();

  otaManager.begin(DEVICE_ID_FROM_EEPROM, SW_VERSION_FROM_EEPROM);

  displayManager.showTempMessage("Ready");
  delay(600);

  // if(otaManager.checkForUpdate()) otaManager.downloadUpdate();
  otaManager.checkForUpdate();
}

void loop() {
  unsigned long now = millis();

  // Button handling
  button.loop();
  if (button.wasPressed()) {
    Serial.println("Button pressed -> trigger blink");
    String peer = server.getFirstPeer();
    if (peer.length() == 0) {
      Serial.println("No peer available to blink.");
      displayManager.showTempMessage("No device");
    } else {
      bool ok = server.triggerBlink(String(DEVICE_ID_FROM_EEPROM), peer);
      if (ok) {
        Serial.printf("Sent blink -> %s\n", peer.c_str());
        displayManager.showTempMessage("Blink sent");
      } else {
        Serial.println("Failed to send blink");
        displayManager.showTempMessage("Blink fail");
      }
    }
  }

  // ===== Periodic re-register =====
  if (now - lastRegister >= REGISTER_INTERVAL) {
    lastRegister = now;
    server.registerDevice(String(DEVICE_ID_FROM_EEPROM));
  }

  // ===== Poll blink events ======
  if (now - lastBlinkPoll >= BLINK_POLL_INTERVAL) {
    lastBlinkPoll = now;
    ServerClient::BlinkInfo info = server.pollBlink(String(DEVICE_ID_FROM_EEPROM));
    if (info.blink) {
      Serial.printf("Blink event from %s\n", info.from.c_str());
      displayManager.startBlinking(2000);  // blink for 2s visually
      // flash LED quickly
      for (int i = 0; i < 6; ++i) {
        digitalWrite(LED_PIN, LOW);
        delay(80);
        digitalWrite(LED_PIN, HIGH);
        delay(80);
      }
    }
  }

  // ===== Periodic event fetch =====
  if (now - lastEventFetch >= EVENT_FETCH_INTERVAL) {
    lastEventFetch = now;
    eventClock.fetchEventFromServer();
  }

  // ===== Periodic OTA Update check =====
  if (now - lastOTACheck >= OTA_CHECK_INTERVAL){
    lastOTACheck = now;
    otaManager.checkForUpdate();
  }

  // ========== MAIN UPDATE LOOP ==========
  eventClock.update();
  displayManager.updateDisplay(
    eventClock.getWeeks(),
    eventClock.getDays(),
    eventClock.getHours(),
    eventClock.getMinutes(),
    WiFi.RSSI(),
    server.isServerReachable());

  wifiManager.loop();

  delay(20);
}