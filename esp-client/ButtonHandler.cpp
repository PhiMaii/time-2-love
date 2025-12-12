#include "ButtonHandler.h"

ButtonHandler::ButtonHandler(uint8_t pin) : _pin(pin), _lastState(HIGH), _pressed(false), _lastDebounce(0) {}

void ButtonHandler::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

void ButtonHandler::loop() {
  bool reading = digitalRead(_pin);

  // Serial.print("Reading: ");
  // Serial.println(reading);

  // Serial.print("Prev: ");
  // Serial.println(_lastState);

  if (reading != _lastState) {
    _lastDebounce = millis();
  }
  // if ((millis() - _lastDebounce) > DEBOUNCE_MS) {
    if (_lastState == HIGH && reading == LOW) {
      _pressed = true;
      // Serial.println("PRESSED!!!!!!!!!!!!!!!!!!!");
    }
  // }
  _lastState = reading;
}

bool ButtonHandler::wasPressed() {
  if (_pressed) {
    _pressed = false;
    return true;
  }
  return false;
}