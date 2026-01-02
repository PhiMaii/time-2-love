// BootManager.h

#ifndef BOOTMANAGER_H
#define BOOTMANAGER_H

#include <Arduino.h>
#include "DisplayManager.h"

class BootManager {
public:
  enum BootState {
    BOOT_START = 0,
    DISPLAY_INIT,
    WIFI_CONNECT,
    TIME_SYNC,
    SERVER_INIT,
    EVENT_FETCH,
    BUTTON_INIT,
    OTA_INIT,
    BOOT_READY,
    BOOT_STATE_COUNT   // keep this last
  };

  BootManager(DisplayManager& display);

  // Set current boot state + label, automatically updates display
  void setState(BootState state, const char* label);

  // Optional: get current state/percent
  BootState getState() const { return _state; }
  uint8_t   getPercent() const { return _percent; }
  const char* getLabel() const { return _label; }

private:
  DisplayManager& _display;
  BootState _state = BOOT_START;
  uint8_t   _percent = 0;
  const char* _label = "Boot";

  void updateDisplay();
};


#endif