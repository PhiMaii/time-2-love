#include "ServerClient.h"
#include "Config.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

ServerClient::ServerClient() {
  _baseUrl = "";
  _peerCount = 0;
  _lastPing = 0;
}

void ServerClient::begin(const char* baseUrl) {
  _baseUrl = String(baseUrl);
}

void ServerClient::parsePeersFromResponse(const String& resp) {
  _peerCount = 0;
  int s = resp.indexOf('[');
  int e = resp.indexOf(']');
  if (s < 0 || e <= s) return;
  String inner = resp.substring(s + 1, e);
  int i = 0;
  while (i < inner.length()) {
    int q1 = inner.indexOf('"', i);
    if (q1 < 0) break;
    int q2 = inner.indexOf('"', q1 + 1);
    if (q2 < 0) break;
    String p = inner.substring(q1 + 1, q2);
    if (_peerCount < 8) _peers[_peerCount++] = p;
    i = q2 + 1;
  }
}

bool ServerClient::registerDevice(const String& device) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[ServerClient] Cannot register: WiFi not connected");
    return false;
  }

  WiFiClient wifi;
  HTTPClient http;

  String url = _baseUrl + "/register";
  http.begin(wifi, url);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"device\":\"" + device + "\"}";
  int code = http.POST(body);

  if (code == 200) {
    String resp = http.getString();
    parsePeersFromResponse(resp);
    Serial.printf("[ServerClient] Registered, peers: ");
    for (uint8_t i = 0; i < _peerCount; ++i) {
      Serial.printf("%s ", _peers[i].c_str());
    }
    Serial.println();
    http.end();
    return true;
  } else {
    Serial.printf("[ServerClient] register failed: HTTP %d\n", code);
    http.end();
    return false;
  }
}

bool ServerClient::triggerBlink(const String& from, const String& to) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClient wifi;
  HTTPClient http;

  String url = _baseUrl + "/blink";
  http.begin(wifi, url);
  http.addHeader("Content-Type", "application/json");

  String body = "{\"from\":\"" + from + "\",\"to\":\"" + to + "\"}";
  int code = http.POST(body);
  if (code == 200) {
    Serial.println("[ServerClient] triggerBlink ok");
    http.end();
    return true;
  } else {
    Serial.printf("[ServerClient] triggerBlink failed: HTTP %d\n", code);
    http.end();
    return false;
  }
}

ServerClient::BlinkInfo ServerClient::pollBlink(const String& device) {
  BlinkInfo info;
  if (WiFi.status() != WL_CONNECTED) return info;

  WiFiClient wifi;
  HTTPClient http;

  String url = _baseUrl + "/blink?device=" + device;
  http.begin(wifi, url);
  int code = http.GET();
  if (code == 200) {
    String resp = http.getString();
    if (resp.indexOf("\"blink\":true") >= 0) {
      info.blink = true;
      int f = resp.indexOf("\"from\":\"");
      if (f >= 0) {
        int start = f + 8;
        int end = resp.indexOf('"', start + 1);
        if (end > start) {
          // substring offsets: start currently at index of first quote of "from":"
          // we need the text between quotes after : -> compute properly
          // locate actual value start
          int valStart = resp.indexOf('"', f + 1);
          if (valStart >= 0) {
            valStart = resp.indexOf('"', valStart + 1);
            if (valStart >= 0) {
              // simpler: find pattern "\"from\":\"" then value begins at f+8+1
            }
          }
          // simpler approach:
          int needle = resp.indexOf("\"from\":\"");
          if (needle >= 0) {
            int vs = needle + 8 + 1; // after "from":"
            int ve = resp.indexOf('"', vs);
            if (ve > vs) {
              info.from = resp.substring(vs, ve);
            }
          }
        }
      }
    }
  }
  http.end();
  return info;
}

String ServerClient::getFirstPeer() {
  if (_peerCount > 0) return _peers[0];
  return "";
}

bool ServerClient::isServerReachable() {
  if (WiFi.status() != WL_CONNECTED) return false;
  if (millis() - _lastPing < 5000) return true; // avoid spamming
  _lastPing = millis();

  WiFiClient wifi;
  HTTPClient http;
  String url = _baseUrl + "/";
  http.begin(wifi, url);
  int code = http.GET();
  http.end();
  return (code == 200);
}