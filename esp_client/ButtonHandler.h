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

private:
  uint8_t _pin;
  bool _lastState;
  bool _pressed;
  unsigned long _lastDebounce;
  const unsigned long DEBOUNCE_MS = 50;
};

#endif