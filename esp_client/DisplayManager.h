// DisplayManager.h

#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Arduino.h>

enum class DisplayState {
  MAIN_LOOP,     // Main loop
  TEMP_MESSAGE,  // While displaying a temp message

  UPDATING,  // State during FW update
  BOOTING,   // State while booting
  ERROR,
  SLEEP
};

class DisplayManager {
public:
  DisplayManager();

  void begin();

  // State handling
  void setState(DisplayState state);
  DisplayState getState() const;

  // Optional helpers
  void startBlinking(unsigned long ms);
  void showTempMessage(const char* msg, unsigned long durationMs = 2000);

  // Main functions
  void updateDisplay(long weeks, long days, long hours, long minutes, int rssi, bool serverUp);

  static void onUpdateProgress(int progress, int total);

  // void setBootProgress(uint8_t percent, const char* label);
  void drawBootProgress(uint8_t percent, const char* label);


private:
  DisplayState _state;
  unsigned long _blinkUntil;
  unsigned long _tempUntil;
  String _tempMsg;
  unsigned long _lastDrawMs;

  // uint8_t _bootPercent;
  // const char* _bootLabel;

  void drawStatusBar(int rssi, bool serverUp);
  void drawMain(long weeks, long days, long hours, long minutes);
  // void drawUpdating();
  void drawError();
  void drawMessage();
  void drawSleep();
};

#endif