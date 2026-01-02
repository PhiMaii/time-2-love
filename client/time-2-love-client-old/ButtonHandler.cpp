// ButtonHandler.cpp

#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t pin)
  : _pin(pin),
    _stableState(HIGH),
    _lastReading(HIGH),
    _longPressEvent(false),
    _shortPressEvent(false),
    _longPressFired(false),        // 🔑 NEW
    _lastDebounceTime(0),
    _pressStartTime(0) {}

void ButtonHandler::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

void ButtonHandler::loop() {
  bool reading = digitalRead(_pin);
  unsigned long now = millis();

  // ---------- Debounce ----------
  if (reading != _lastReading) {
    _lastDebounceTime = now;
    _lastReading = reading;
  }

  if ((now - _lastDebounceTime) < DEBOUNCE_MS) {
    return;
  }

  // ---------- Stable state change ----------
  if (reading != _stableState) {
    _stableState = reading;

    // Button pressed
    if (_stableState == LOW) {
      _pressStartTime = now;
      _longPressFired = false;     // reset for new press
    }

    // Button released
    else {
      if (!_longPressFired &&
          (now - _pressStartTime) < LONG_PRESS_MS) {
        _shortPressEvent = true;
      }
    }
  }

  // ---------- Long press detection (ONE-SHOT) ----------
  if (_stableState == LOW &&
      !_longPressFired &&
      (now - _pressStartTime) >= LONG_PRESS_MS) {
    _longPressFired = true;
    _longPressEvent = true;
  }
}

bool ButtonHandler::wasPressed() {
  if (_shortPressEvent) {
    _shortPressEvent = false;
    return true;
  }
  return false;
}

bool ButtonHandler::wasLongPressed() {
  if (_longPressEvent) {
    _longPressEvent = false;   // consume event
    return true;
  }
  return false;
}