#ifndef LEDMANAGER_H
#define LEDMANAGER_H
#pragma once

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "Config.h"

class LEDManager {
public:
  LEDManager(uint16_t numLeds, uint8_t dataPin, neoPixelType type = NEO_GRB + NEO_KHZ800);

  void begin();

  // Start one double-heartbeat sequence (non-blocking)
  void startHeartbeat(uint32_t color = 0xFF0000); // default red

  // Call this frequently from loop()
  void loop();

private:
  Adafruit_NeoPixel strip;
  uint16_t _numLeds;
  uint8_t  _dataPin;

  // Heartbeat state machine
  enum HeartbeatPhase {
    HB_IDLE,
    HB_BEAT1_UP,
    HB_BEAT1_HOLD,
    HB_BEAT1_DOWN,
    HB_PAUSE_BETWEEN,
    HB_BEAT2_UP,
    HB_BEAT2_HOLD,
    HB_BEAT2_DOWN,
    HB_REST_AFTER
  };

  HeartbeatPhase _phase = HB_IDLE;

  uint32_t _hbColor = 0xFF0000;
  uint8_t  _brightness = 0;
  unsigned long _lastStepTime = 0;

  // use config defines
  static const uint16_t _upTime      = HB_UP_TIME_MS;
  static const uint16_t _downTime    = HB_DOWN_TIME_MS;
  static const uint16_t _holdTime    = HB_HOLD_TIME_MS;
  static const uint16_t _betweenBeatsPause = HB_BETWEEN_BEATS_MS;
  static const uint16_t _restTime    = HB_REST_AFTER_MS;

  static const uint8_t  _stepSize    = HB_STEP_SIZE;

  void fillAll(uint32_t color, uint8_t brightness);
  void updateHeartbeat();
};


#endif
