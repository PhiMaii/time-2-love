#include "DisplayManager.h"
#include <U8g2lib.h>
#include <Wire.h>

// SSD1306 128x32 I2C constructor
static U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

DisplayManager::DisplayManager() {
  _blinkUntil = 0;
  _tempUntil = 0;
  _tempMsg = "";
  _lastDrawMs = 0;
}

void DisplayManager::begin() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 10, "Init...");
  u8g2.sendBuffer();
}

void DisplayManager::showTempMessage(const char* msg) {
  _tempMsg = String(msg);
  _tempUntil = millis() + 2000;
}

void DisplayManager::startBlinking(unsigned long ms) {
  _blinkUntil = millis() + ms;
}

void DisplayManager::updateDisplay(long weeks, long days, long hours, long minutes, int rssi, bool serverUp) {
  if (millis() - _lastDrawMs < 60) return; // throttle ~16fps
  _lastDrawMs = millis();

  u8g2.clearBuffer();

  // WiFi bars top-left
  int bars = 0;
  // if (rssi > -60) bars = 4;
  // else if (rssi > -70) bars = 3;
  // else if (rssi > -80) bars = 2;
  // else if (rssi > -90) bars = 1;
  // else bars = 3;
  bars = 4;
  for (int i = 0; i < bars; ++i) {
    int x = 2 + i * 3;
    int h = 3 + i * 2;
    u8g2.drawBox(x, 0 + (10 - h), 2, h);
  }

  // Server status top-right
  if (serverUp) {
    u8g2.drawBox(120, 2, 6, 6);
  }
  else u8g2.drawFrame(120, 2, 6, 6);

  // if temp message
  if (_tempUntil > millis()) {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 18, _tempMsg.c_str());
    u8g2.sendBuffer();
    return;
  }

  // main countdown text
  if (weeks == 0 && days == 0 && hours == 0 && minutes == 0) {
    u8g2.setFont(u8g2_font_6x10_tf); // old font
    u8g2.drawStr(0, 16, "No event / passed");
  } else {
    char buf[48];
    snprintf(buf, sizeof(buf), "%ldw %ldt %ldh %ldm", weeks, days, hours, minutes);
    // u8g2.setFont(u8g2_font_7x13_mf);
    u8g2.setFont(u8g2_font_10x20_tf);
    u8g2.drawStr(2, 31, buf);
  }

  // blinking overlay
  if (millis() < _blinkUntil) {
    if ((millis() / 200) % 2 == 0) {
      u8g2.setDrawColor(1);
      u8g2.drawBox(0, 0, 128, 32);
      u8g2.setDrawColor(0);
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.drawStr(36, 18, "BLINK");
      u8g2.setDrawColor(1);
    }
  }

  u8g2.sendBuffer();
}