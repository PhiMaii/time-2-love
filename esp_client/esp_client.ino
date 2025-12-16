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
#include "LEDManager.h"
#include "BootManager.h"
#include "DelayedCaller.h"

// Globals
ServerClient server;
EventClock eventClock;
DisplayManager displayManager;

ButtonHandler blinkButton(BLINK_BUTTON_PIN);
ButtonHandler sleepButton(SLEEP_BUTTON_PIN);
LEDManager ledManager(NUM_LEDS, LED_STRIP_PIN);

EEPROMManager eepromManager;
OTAManager otaManager;
WiFiManager wifiManager;

BootManager bootManager(displayManager);

DelayedCaller delayedCaller;


// Timing variables
unsigned long lastEventFetch = 0;
unsigned long lastBlinkPoll = 0;
unsigned long lastRegister = 0;
unsigned long lastOTACheck = 0;

bool waitingForResponse = false;

// Global version variables
String DEVICE_ID_FROM_EEPROM = "";
String SW_VERSION_FROM_EEPROM = "";

void setup() {
  // delay(100);
  Serial.begin(115200);

  delay(500);

  // displayManager.setState(DisplayState::BOOTING);
  bootManager.setState(BootManager::BOOT_START, "EEPROM ...");
  eepromManager.begin();
  eepromManager.loadOrInitialize();

  DEVICE_ID_FROM_EEPROM = eepromManager.getDeviceId();
  SW_VERSION_FROM_EEPROM = eepromManager.getSwVersion();

  Serial.println();
  Serial.print("Starting device " + DEVICE_ID_FROM_EEPROM + "\n");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // builtin LED active LOW on many boards

  bootManager.setState(BootManager::DISPLAY_INIT, "Display...");
  displayManager.begin();
  displayManager.showTempMessage("Booting...");

  // WiFi
  bootManager.setState(BootManager::WIFI_CONNECT, "WiFi...");
  wifiManager.begin();

  bootManager.setState(BootManager::TIME_SYNC, "Time sync...");
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  Serial.println("NTP configured");

  bootManager.setState(BootManager::SERVER_INIT, "Server...");
  server.begin(SERVER_URL);
  server.registerDevice(String(DEVICE_ID_FROM_EEPROM));

  bootManager.setState(BootManager::EVENT_FETCH, "Event...");
  eventClock.fetchEventFromServer();

  bootManager.setState(BootManager::BUTTON_INIT, "Buttons...");
  blinkButton.begin();
  sleepButton.begin();
  ledManager.begin();

  bootManager.setState(BootManager::OTA_INIT, "OTA...");
  otaManager.begin(DEVICE_ID_FROM_EEPROM, SW_VERSION_FROM_EEPROM);

  bootManager.setState(BootManager::BOOT_READY, "Ready");
  // displayManager.setState(DisplayState::MAIN_LOOP);
  // displayManager.showTempMessage("Ready");
  delay(600);

  // if(otaManager.checkForUpdate()) otaManager.downloadUpdate();
  otaManager.checkForUpdate();

  lastEventFetch = millis();
  lastBlinkPoll = millis();
  lastRegister = millis();
  lastOTACheck = millis();
}

void loop() {
  unsigned long now = millis();

  // Button handling
  blinkButton.loop();
  sleepButton.loop();

  if (sleepButton.wasLongPressed()) {
    displayManager.showTempMessage("Searching ...");
    if (otaManager.checkForUpdate()) {
      // displayManager.showTempMessage("Update found!");
      // delay(1000);
      displayManager.showTempMessage("Installing ...");
      delayedCaller.callWithDelay(otaManager.downloadUpdate, 1000);


      // waitingForResponse = true;
      // while (!(sleepButton.wasPressed() || blinkButton.wasPressed())) {
      //   if (sleepButton.wasLongPressed()) {
      //     displayManager.showTempMessage("Installing ...");
      //     break;
      //   }
      //   delay(20);
      // }
    } else {

      displayManager.showTempMessage("Up to date");
    }
    // if(otaManager.checkForUpdate()) otaManager.downloadUpdate();
    // Serial.println("long press detected");
  }


  if (blinkButton.wasPressed()) {
    Serial.println("Blink Button pressed -> trigger blink");
    String peer = server.getFirstPeer();
    if (peer.length() == 0) {
      Serial.println("No peer available to blink.");
      displayManager.showTempMessage("No device");
    } else {
      bool ok = server.triggerBlink(String(DEVICE_ID_FROM_EEPROM), peer);
      if (ok) {
        Serial.printf("Sent blink -> %s\n", peer.c_str());
        ledManager.startHeartbeat();
        displayManager.showTempMessage("Blink sent");
      } else {
        Serial.println("Failed to send blink");
        displayManager.showTempMessage("Blink fail");
      }
    }
  }

  if (sleepButton.wasPressed()) {
    DisplayState state = displayManager.getState();
    if (state != DisplayState::SLEEP) {
      Serial.println("Sleep Button pressed -> goto sleep");
      displayManager.setState(DisplayState::SLEEP);
    } else if (state == DisplayState::SLEEP) {
      Serial.println("Already sleeping -> waking up");
      displayManager.setState(DisplayState::MAIN_LOOP);
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
      ledManager.startHeartbeat();
      displayManager.startBlinking(2000);  // blink for 2s visually
    }
  }

  // ===== Periodic event fetch =====
  if (now - lastEventFetch >= EVENT_FETCH_INTERVAL) {
    lastEventFetch = now;
    eventClock.fetchEventFromServer();
  }

  // ===== Periodic OTA Update check =====
  if (now - lastOTACheck >= OTA_CHECK_INTERVAL) {
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
  ledManager.loop();
  delayedCaller.update();

  delay(MAIN_LOOP_DELAY);
}