// ButtonHandler.h

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>

class ButtonHandler {
public:
  ButtonHandler(uint8_t pin);

  void begin();
  void loop();

  bool wasPressed();       // Short press
  bool wasLongPressed();   // Long press (> LONG_PRESS_MS)

private:
  uint8_t _pin;

  bool _stableState;       // debounced state
  bool _lastReading;       // raw reading
  bool _longPressEvent;    // latched event
  bool _longPressFired;   // prevents repeated long-press events
  bool _shortPressEvent;   // latched event

  unsigned long _lastDebounceTime;
  unsigned long _pressStartTime;

  static constexpr unsigned long DEBOUNCE_MS   = 50;
  static constexpr unsigned long LONG_PRESS_MS = 1000;
};

#endif