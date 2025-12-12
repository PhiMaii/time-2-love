#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <time.h>

#include "Config.h"
#include "ServerClient.h"
#include "EventClock.h"
#include "DisplayManager.h"
#include "ButtonHandler.h"
#include "EEPROMManager.h"


// Globals
ServerClient server;
EventClock eventClock;
DisplayManager displayManager;
ButtonHandler button(BUTTON_PIN);

unsigned long lastEventFetch = 0;
unsigned long lastBlinkPoll = 0;
unsigned long lastRegister = 0;

String DEVICE_ID = "";

void setup() {
  Serial.begin(115200);

  EEPROMManager::begin();
  EEPROMManager::loadOrInitialize();

  DEVICE_ID = EEPROMManager::getDeviceId();
  String swVersion = EEPROMManager::getSwVersion();

  Serial.println("Device ID from EEPROM: " + DEVICE_ID);
  Serial.println("Software Version: " + swVersion);
  Serial.println("######################");

  delay(500);

  Serial.println();
  Serial.print("Starting device " + DEVICE_ID + "\n");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); // builtin LED active LOW on many boards

  displayManager.begin();
  displayManager.showTempMessage("Booting...");

  // WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
    delay(250);
    Serial.print(".");
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.printf("WiFi connected, IP: %s\n", WiFi.localIP().toString().c_str());
    // NTP
    configTime(0, 0, "pool.ntp.org", "time.google.com");
    Serial.println("NTP configured");
  } else {
    Serial.println();
    Serial.println("WiFi connection failed (timeout)");
  }

  server.begin(SERVER_URL);
  server.registerDevice(String(DEVICE_ID));

  eventClock.fetchEventFromServer();

  lastEventFetch = millis();
  lastBlinkPoll = millis();
  lastRegister = millis();

  button.begin();

  displayManager.showTempMessage("Ready");
  delay(600);
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
      displayManager.showTempMessage("No peer");
    } else {
      bool ok = server.triggerBlink(String(DEVICE_ID), peer);
      if (ok) {
        Serial.printf("Sent blink -> %s\n", peer.c_str());
        displayManager.showTempMessage("Blink sent");
      } else {
        Serial.println("Failed to send blink");
        displayManager.showTempMessage("Blink fail");
      }
    }
  }

  // Periodic re-register
  if (now - lastRegister >= REGISTER_INTERVAL) {
    lastRegister = now;
    server.registerDevice(String(DEVICE_ID));
  }

  // Poll blink events
  if (now - lastBlinkPoll >= BLINK_POLL_INTERVAL) {

    // Serial.println(WiFi.RSSI());

    lastBlinkPoll = now;
    ServerClient::BlinkInfo info = server.pollBlink(String(DEVICE_ID));
    if (info.blink) {
      Serial.printf("Blink event from %s\n", info.from.c_str());
      displayManager.startBlinking(3000); // blink for 3s visually
      // flash LED quickly
      for (int i = 0; i < 6; ++i) {
        digitalWrite(LED_PIN, LOW); delay(80);
        digitalWrite(LED_PIN, HIGH); delay(80);
      }
    }
  }

  // Periodic event fetch
  if (now - lastEventFetch >= EVENT_FETCH_INTERVAL) {
    lastEventFetch = now;
    eventClock.fetchEventFromServer();
  }

  // Update clock/time, then display
  eventClock.update();
  displayManager.updateDisplay(
    eventClock.getWeeks(),
    eventClock.getDays(),
    eventClock.getHours(),
    WiFi.RSSI(),
    server.isServerReachable()
  );

  delay(20);
}