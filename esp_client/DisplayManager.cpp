#include "WString.h"
#include "EEPROMManager.h"
#include "core_esp8266_features.h"
#include <cstdio>
#include "DisplayManager.h"
#include <U8g2lib.h>
#include <Wire.h>
#include "OTAManager.h"

#include <avr/pgmspace.h>


// SSD1306 128x32 I2C constructor
static U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

DisplayManager::DisplayManager()
  : _state(DisplayState::MAIN_LOOP),
    _blinkUntil(0),
    _tempUntil(0),
    _tempMsg(""),
    _lastDrawMs(0) {}

void DisplayManager::begin() {
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 10, "Init... v0.1.2");
  // u8g2.drawBitmap(0, 0, STARTUP_SCREEN_INVERTED_WIDTH, STARTUP_SCREEN_INVERTED_HEIGHT, startup_screen_inverted);
  u8g2.sendBuffer();
}

void DisplayManager::setState(DisplayState state) {
  _state = state;
}

DisplayState DisplayManager::getState() const {
  return _state;
}

void DisplayManager::showTempMessage(const char* msg, unsigned long durationMs) {
  _tempMsg = msg;
  _tempUntil = millis() + durationMs;
  _state = DisplayState::TEMP_MESSAGE;
}

void DisplayManager::startBlinking(unsigned long ms) {
  _blinkUntil = millis() + ms;
  // _state = DisplayState::BLINK;
}

void DisplayManager::updateDisplay(long weeks, long days, long hours, long minutes, int rssi, bool serverUp) {
  if (millis() - _lastDrawMs < 60) return;
  _lastDrawMs = millis();

  // auto return from MESSAGE
  if (_state == DisplayState::TEMP_MESSAGE && millis() > _tempUntil) {
    _state = DisplayState::MAIN_LOOP;
  }

  u8g2.clearBuffer();
  drawStatusBar(rssi, serverUp);

  switch (_state) {
    case DisplayState::MAIN_LOOP:
      drawMain(weeks, days, hours, minutes);
      break;

    case DisplayState::TEMP_MESSAGE:
      drawMessage();
      break;

    case DisplayState::UPDATING:
      // drawUpdating();
      break;

    case DisplayState::ERROR:
      // drawError();
      break;

    case DisplayState::SLEEP:
      // drawSleep();
      break;

      // case DisplayState::BLINK:
      //   drawBlink();
      //   break;
  }

  // blinking overlay (state-independent)
  if (millis() < _blinkUntil && ((millis() / 200) % 2 == 0)) {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, 128, 32);
    u8g2.setDrawColor(0);
    u8g2.drawStr(36, 18, "BLINK");
    u8g2.setDrawColor(1);
  }

  u8g2.sendBuffer();
}


/* ---------- Draw helper ---------- */

void DisplayManager::drawStatusBar(int rssi, bool serverUp) {
  // int bars = (rssi > -50) ? 4 :
  //            (rssi > -60) ? 3 :
  //            (rssi > -70) ? 2 :
  //            (rssi > -80) ? 1 : 0;

  int bars = 4;

  for (int i = 0; i < bars; ++i) {
    int x = 2 + i * 3;
    int h = 3 + i * 2;
    u8g2.drawBox(x, (10 - h), 2, h);
  }

  u8g2.setFont(u8g2_font_6x10_tf);
  // char buf[48];
  const String version = EEPROMManager::getSwVersion();
  // snprintf(buf, sizeof(buf), "v%c", version);
  int textWidth = u8g2.getStrWidth(version.c_str());
  int x = (u8g2.getDisplayWidth() - textWidth) / 2;
  u8g2.drawStr(x + 5, 10, version.c_str());

  // static const unsigned char PROGMEM update_icon[] = { 0x40, 0x0, 0xe2, 0x5f, 0x20, 0x41, 0x26, 0x48, 0xa0, 0x7f, 0x4, 0x20, 0x0 };
  // u8g2.drawXBMP(80, 11, 10, 10, update_icon);
  if(OTAManager::getNewSwVersion() != "null") u8g2.drawDisc(x - 5, 5, 3);
  // else u8g2.drawCircle(x - 5, 5, 3);

  if (serverUp) u8g2.drawBox(120, 2, 6, 6);
  else u8g2.drawFrame(120, 2, 6, 6);
}

void DisplayManager::drawMain(long weeks, long days, long hours, long minutes) {
  if (weeks == 0 && days == 0 && hours == 0 && minutes == 0) {
    u8g2.setFont(u8g2_font_6x10_tf);  // old font
    u8g2.drawStr(0, 16, "No event / passed");
  } else {
    char buf[48];
    snprintf(buf, sizeof(buf), "%ldw %ldt %ldh %ldm", weeks, days, hours, minutes);
    u8g2.setFont(u8g2_font_10x20_tf);

    // Get display width and text width, then center
    int textWidth = u8g2.getStrWidth(buf);
    int x = (u8g2.getDisplayWidth() - textWidth) / 2;
    u8g2.drawStr(x, 30, buf);
  }
}

void DisplayManager::drawUpdating() {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(30, 18, "Updating...");
}

void DisplayManager::drawMessage() {
  u8g2.setFont(u8g2_font_10x20_tf);

  int textWidth = u8g2.getStrWidth(_tempMsg.c_str());
  int x = (u8g2.getDisplayWidth() - textWidth) / 2;

  u8g2.drawStr(x, 29, _tempMsg.c_str());
  u8g2.sendBuffer();
}

void DisplayManager::drawError() {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(32, 18, "ERROR");
}

// ###############

// void DisplayManager::showTempMessage(const char* msg) {
//   _tempMsg = String(msg);
//   _tempUntil = millis() + 2000;
// }

// void DisplayManager::startBlinking(unsigned long ms) {
//   _blinkUntil = millis() + ms;
// }

// void DisplayManager::updateDisplay(long weeks, long days, long hours, long minutes, int rssi, bool serverUp) {
//   if (millis() - _lastDrawMs < 60) return;  // throttle ~16fps
//   _lastDrawMs = millis();

//   u8g2.clearBuffer();

//   // WiFi bars top-left
//   int bars = 0;
//   // if (rssi > -60) bars = 4;
//   // else if (rssi > -70) bars = 3;
//   // else if (rssi > -80) bars = 2;
//   // else if (rssi > -90) bars = 1;
//   // else bars = 3;
//   bars = 4;
//   for (int i = 0; i < bars; ++i) {
//     int x = 2 + i * 3;
//     int h = 3 + i * 2;
//     u8g2.drawBox(x, 0 + (10 - h), 2, h);
//   }

//   // Server status top-right
//   if (serverUp) {
//     u8g2.drawBox(120, 2, 6, 6);
//   } else u8g2.drawFrame(120, 2, 6, 6);

//   // if temp message
//   if (_tempUntil > millis()) {
//     // u8g2.setFont(u8g2_font_6x10_tf);
//     u8g2.setFont(u8g2_font_10x20_tf);

//     int textWidth = u8g2.getStrWidth(_tempMsg.c_str());
//     int x = (u8g2.getDisplayWidth() - textWidth) / 2;

//     u8g2.drawStr(x, 27, _tempMsg.c_str());
//     u8g2.sendBuffer();
//     return;
//   }

//   // main countdown text
//   if (weeks == 0 && days == 0 && hours == 0 && minutes == 0) {
//     u8g2.setFont(u8g2_font_6x10_tf);  // old font
//     u8g2.drawStr(0, 16, "No event / passed");
//   } else {
//     char buf[48];
//     snprintf(buf, sizeof(buf), "%ldw %ldt %ldh %ldm", weeks, days, hours, minutes);
//     u8g2.setFont(u8g2_font_10x20_tf);

//     // Get display width and text width, then center
//     int textWidth = u8g2.getStrWidth(buf);
//     int x = (u8g2.getDisplayWidth() - textWidth) / 2;
//     u8g2.drawStr(x, 31, buf);
//   }

//   // blinking overlay
//   if (millis() < _blinkUntil) {
//     if ((millis() / 200) % 2 == 0) {
//       u8g2.setDrawColor(1);
//       u8g2.drawBox(0, 0, 128, 32);
//       u8g2.setDrawColor(0);
//       // u8g2.setFont(u8g2_font_6x10_tf);
//       u8g2.setFont(u8g2_font_10x20_tf);
//       u8g2.drawStr(36, 26, "BLINK");
//       u8g2.setDrawColor(1);
//     }
//   }

//   u8g2.sendBuffer();
// }

// void DisplayManager::displayUpdateProgress(int progress, int total) {
//   String oldVersion = "0.1.0";
//   String newVersion = "0.1.2";

//   u8g2.clearBuffer();
//   u8g2.setFont(u8g2_font_6x10_tf);
//   int progressPercent = (100 * progress) / total;

//   char progress_buf[48];
//   snprintf(progress_buf, sizeof(progress_buf), "%i/%i  %i%", progress, total, progressPercent);

//   char version_buf[48];
//   snprintf(version_buf, sizeof(version_buf), "%s -> %s", oldVersion, newVersion);

//   u8g2.drawStr(10, 10, "Updating FW ...");
//   u8g2.drawStr(10, 30, version_buf);
//   u8g2.drawStr(10, 20, progress_buf);

//   u8g2.sendBuffer();
// }