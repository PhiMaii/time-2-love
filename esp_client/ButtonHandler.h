// ButtonHandler.h

#ifndef BUTTONHANDLER_H
#define BUTTONHANDLER_H

#include <Arduino.h>

class ButtonHandler {
public:
  ButtonHandler(uint8_t pin);
  void begin();
  void loop();
  bool wasPressed();
  bool wasLongPressed();  // New: Long press (>1000ms)

private:
  uint8_t _pin;
  bool _lastState;
  bool _pressed;
  bool _longPressed;

  unsigned long _lastDebounce;
  unsigned long _pressStartTime;
  const unsigned long DEBOUNCE_MS = 50;
  const unsigned long LONG_PRESS_MS = 1000;
};

#endif