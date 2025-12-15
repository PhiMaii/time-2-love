// ButtonHandler.cpp

#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t pin)
  : _pin(pin), _lastState(HIGH), _pressed(false), _lastDebounce(0) {}

void ButtonHandler::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

void ButtonHandler::loop() {
  bool reading = digitalRead(_pin);

  if (reading != _lastState) {
    _lastDebounce = millis();
  }
  if (_lastState == HIGH && reading == LOW) {
    _pressed = true;
  }
  _lastState = reading;
}

bool ButtonHandler::wasPressed() {
  if (_pressed) {
    _pressed = false;
    return true;
  }
  return false;
}