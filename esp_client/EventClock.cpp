#include "EventClock.h"
#include "Config.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <time.h>

EventClock::EventClock() {
  _eventEpoch = 0;
  _now = 0;
}

void EventClock::fetchEventFromServer() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[EventClock] WiFi not connected, cannot fetch event");
    return;
  }

  WiFiClient wifi;
  HTTPClient http;

  String url = String(SERVER_URL) + "/event";
  http.begin(wifi, url);
  int code = http.GET();
  if (code == 200) {
    String resp = http.getString();  // expected {"event":<epoch>}
    int colon = resp.indexOf(':');
    if (colon >= 0) {
      String num = resp.substring(colon + 1);
      num.trim();
      int br = num.indexOf('}');
      if (br >= 0) num = num.substring(0, br);
      long epoch = num.toInt();
      if (epoch > 1000000000L) {
        _eventEpoch = epoch;
        Serial.printf("[EventClock] Event epoch: %ld\n", _eventEpoch);
      } else {
        Serial.println("[EventClock] invalid epoch received");
      }
    }
  } else {
    Serial.printf("[EventClock] event fetch failed: HTTP %d\n", code);
  }
  http.end();
}

void EventClock::update() {
  time_t t;
  time(&t);
  _now = (long)t;
}

long EventClock::getWeeks() {
  if (_eventEpoch == 0 || _now == 0) return 0;
  long diff = _eventEpoch - _now;
  if (diff <= 0) return 0;
  return diff / (7L * 24L * 3600L);
}

long EventClock::getDays() {
  if (_eventEpoch == 0 || _now == 0) return 0;
  long diff = _eventEpoch - _now;
  if (diff <= 0) return 0;
  diff %= (7L * 24L * 3600L);
  return diff / (24L * 3600L);
}

long EventClock::getHours() {
  if (_eventEpoch == 0 || _now == 0) return 0;
  long diff = _eventEpoch - _now;
  if (diff <= 0) return 0;
  diff %= (24L * 3600L);
  return diff / 3600L;
}

long EventClock::getMinutes() {
  if (_eventEpoch == 0 || _now == 0) return 0;
  long diff = _eventEpoch - _now;
  if (diff <= 0) return 0;
  diff %= 3600L;
  return diff / 60L;
}

bool EventClock::hasEvent() {
  return _eventEpoch != 0;
}