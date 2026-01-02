#include "LEDManager.h"

LEDManager::LEDManager(uint16_t numLeds, uint8_t dataPin, neoPixelType type)
  : strip(numLeds, dataPin, type),
    _numLeds(numLeds),
    _dataPin(dataPin) {
}

void LEDManager::begin() {
  strip.begin();
  strip.show();
  strip.setBrightness(255);
}

void LEDManager::fillAll(uint32_t color, uint8_t brightness) {
  strip.setBrightness(brightness);
  for (uint16_t i = 0; i < _numLeds; i++) {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void LEDManager::startHeartbeat(uint32_t color) {
  _hbColor = color;
  _brightness = 0;
  _phase = HB_BEAT1_UP;
  _lastStepTime = millis();
}

void LEDManager::loop() {
  if (_phase == HB_IDLE) return;
  updateHeartbeat();
}

void LEDManager::updateHeartbeat() {
  unsigned long now = millis();

  switch (_phase) {
    case HB_BEAT1_UP: {
      unsigned long interval = _upTime / (255 / _stepSize + 1);
      if (now - _lastStepTime >= interval) {
        _lastStepTime = now;
        if (_brightness + _stepSize >= 255) {
          _brightness = 255;
          fillAll(_hbColor, _brightness);
          _phase = HB_BEAT1_HOLD;
          _lastStepTime = now;
        } else {
          _brightness += _stepSize;
          fillAll(_hbColor, _brightness);
        }
      }
      break;
    }

    case HB_BEAT1_HOLD:
      if (now - _lastStepTime >= _holdTime) {
        _phase = HB_BEAT1_DOWN;
        _lastStepTime = now;
      }
      break;

    case HB_BEAT1_DOWN: {
      unsigned long interval = _downTime / (255 / _stepSize + 1);
      if (now - _lastStepTime >= interval) {
        _lastStepTime = now;
        if (_brightness <= _stepSize) {
          _brightness = 0;
          fillAll(_hbColor, _brightness);
          _phase = HB_PAUSE_BETWEEN;
          _lastStepTime = now;
        } else {
          _brightness -= _stepSize;
          fillAll(_hbColor, _brightness);
        }
      }
      break;
    }

    case HB_PAUSE_BETWEEN:
      if (now - _lastStepTime >= _betweenBeatsPause) {
        _phase = HB_BEAT2_UP;
        _brightness = 0;
        _lastStepTime = now;
      }
      break;

    case HB_BEAT2_UP: {
      unsigned long interval = _upTime / (255 / _stepSize + 1);
      if (now - _lastStepTime >= interval) {
        _lastStepTime = now;
        if (_brightness + _stepSize >= 255) {
          _brightness = 255;
          fillAll(_hbColor, _brightness);
          _phase = HB_BEAT2_HOLD;
          _lastStepTime = now;
        } else {
          _brightness += _stepSize;
          fillAll(_hbColor, _brightness);
        }
      }
      break;
    }

    case HB_BEAT2_HOLD:
      if (now - _lastStepTime >= _holdTime) {
        _phase = HB_BEAT2_DOWN;
        _lastStepTime = now;
      }
      break;

    case HB_BEAT2_DOWN: {
      unsigned long interval = _downTime / (255 / _stepSize + 1);
      if (now - _lastStepTime >= interval) {
        _lastStepTime = now;
        if (_brightness <= _stepSize) {
          _brightness = 0;
          fillAll(_hbColor, _brightness);
          _phase = HB_REST_AFTER;
          _lastStepTime = now;
        } else {
          _brightness -= _stepSize;
          fillAll(_hbColor, _brightness);
        }
      }
      break;
    }

    case HB_REST_AFTER:
      if (now - _lastStepTime >= _restTime) {
        // Turn everything off and go idle
        fillAll(0, 0);
        _phase = HB_IDLE;
      }
      break;

    case HB_IDLE:
    default:
      break;
  }
}
