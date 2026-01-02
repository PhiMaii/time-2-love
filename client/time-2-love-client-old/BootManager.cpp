// BootManager.cpp

#include "BootManager.h"

BootManager::BootManager(DisplayManager& display)
  : _display(display) {
}

void BootManager::setState(BootState state, const char* label) {
  _state  = state;
  _label  = label;

  // Map enum (0..BOOT_STATE_COUNT-1) to 0..100 %
  _percent = static_cast<uint8_t>(
    (static_cast<uint8_t>(_state) * 100) / (BOOT_STATE_COUNT - 1)
  );

  updateDisplay();
}

void BootManager::updateDisplay() {
  // Adapt this to your real DisplayManager API
  // Example: draw progress bar + text
  _display.drawBootProgress(_percent, _label);
}
