// ================= WiFiManager.h =================
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>


class WiFiManager {
public:
  WiFiManager();
  void begin();
  void loop();

private:
  ESP8266WebServer server;
  DNSServer dnsServer;  // ✅ DAS FEHLTE


  void startAP();
  bool connectWiFi(const String& ssid, const String& password);
};

#endif