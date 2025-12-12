#ifndef SERVERCLIENT_H
#define SERVERCLIENT_H

#include <Arduino.h>

class ServerClient {
public:
  struct BlinkInfo {
    bool blink;
    String from;
    long ts;
    BlinkInfo(): blink(false), from(""), ts(0) {}
  };

  ServerClient();
  void begin(const char* baseUrl);
  bool registerDevice(const String& device);
  bool triggerBlink(const String& from, const String& to);
  BlinkInfo pollBlink(const String& device);
  String getFirstPeer();
  bool isServerReachable();

private:
  String _baseUrl;
  String _peers[8];
  uint8_t _peerCount;
  unsigned long _lastPing;
  void parsePeersFromResponse(const String& resp);
};

#endif