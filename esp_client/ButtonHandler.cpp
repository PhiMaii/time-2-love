// ButtonHandler.cpp

#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t pin)
  : _pin(pin), _lastState(HIGH), _pressed(false), _longPressed(false), _lastDebounce(0), _pressStartTime(0) {}

void ButtonHandler::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

void ButtonHandler::loop() {
  bool reading = digitalRead(_pin);

  // Debounce state change
  if (reading != _lastState) {
    _lastDebounce = millis();
  }

  // Button pressed (HIGH->LOW transition)
  if (_lastState == HIGH && reading == LOW) {
    _pressed = true;
    _longPressed = false;
    _pressStartTime = millis();
  }

  // Check for long press while held
  if (_pressed && !_longPressed && (millis() - _pressStartTime > LONG_PRESS_MS)) {
    _longPressed = true;
    Serial.println("LONG PRESS!!!");
  }

  _lastState = reading;
}

bool ButtonHandler::wasPressed() {
  if (_pressed && !_longPressed) {  // Only short press
    _pressed = false;
    return true;
  }
  return false;
}

bool ButtonHandler::wasLongPressed() {
  if (_longPressed) {
    _longPressed = false;
    _pressed = false;  // Clear both
    return true;
  }
  return false;
}