<<<<<<< HEAD
// WiFiManager.h

=======
// ================= WiFiManager.h =================
>>>>>>> d25a20512cd453e968a3f945c35ed3659aa59089
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
<<<<<<< HEAD
=======
  DNSServer dnsServer;  // ✅ DAS FEHLTE

>>>>>>> d25a20512cd453e968a3f945c35ed3659aa59089

  void startAP();
  bool connectWiFi(const String& ssid, const String& password);
};

#endif