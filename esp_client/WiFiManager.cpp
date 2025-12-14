// ================= WiFiManager.cpp =================
#include "WiFiManager.h"
#include "EEPROMManager.h"
#include "Config.h"

#include <DNSServer.h>


WiFiManager::WiFiManager()
  : server(80) {}

void WiFiManager::begin() {
  String ssid = EEPROMManager::getSsid();
  String wifiPassword = EEPROMManager::getWifiPassword();

  if (!ssid.isEmpty() && connectWiFi(ssid, wifiPassword)) {
    Serial.println(String(WIFI_PREFIX) + "WLAN verbunden!");
    Serial.println(WiFi.localIP());
    // connectWiFi(ssid, wifiPassword);
  } else {
    Serial.println("WLAN fehlgeschlagen – starte AP");
    startAP();
  }
}

void WiFiManager::loop() {
  server.handleClient();
}

bool WiFiManager::connectWiFi(const String& ssid, const String& password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print(String(WIFI_PREFIX) + "Attempting to connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 7500) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.printf("[WIFI] WiFi connected, IP: %s", WiFi.localIP().toString().c_str());
    return true;
  }

  Serial.println(String(WIFI_PREFIX) + "WiFi connection failed");
  return false;
}


void WiFiManager::startAP() {
  Serial.println("[WIFI] Switching to AP mode");
  WiFi.mode(WIFI_AP);

  bool apResult = WiFi.softAP("Time2Love-SETUP");
  if (apResult) {
    Serial.println("[WIFI] AP started: Time2Love-SETUP");
    Serial.print("[WIFI] AP IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("[WIFI] Failed to start AP");
  }

  // dnsServer.start(53, "*", WiFi.softAPIP());
  // Serial.println("[WIFI] DNS server started (Captive Portal active)");

  // server.onNotFound([this]() {
  //   server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  //   server.send(302, "text/plain", "");
  // });

  // server.on("/hotspot-detect.html", [this]() {
  //   Serial.println("[WiFiManager] iOS captive check");
  //   server.send(200, "text/html",
  //               "<!DOCTYPE html>"
  //               "<html><head>"
  //               "<meta http-equiv='refresh' content='0; url=/' />"
  //               "</head><body>"
  //               "Redirecting..."
  //               "</body></html>");
  // });

  server.on("/", [this]() {
    Serial.println("[WIFI] Client connected to captive portal");
    server.send(200, "text/html",
                "<h2>WiFi Setup</h2>"
                "<form action='/save'>"
                "SSID:<br><input name='s'><br>"
                "Passwort:<br><input name='p' type='password'><br><br>"
                "<button>Speichern</button>"
                "</form>");
  });

  server.on("/save", [this]() {
    String ssid = server.arg("s");
    String pass = server.arg("p");

    EEPROMManager::setWiFiPassword(pass);
    EEPROMManager::setSsid(ssid);

    Serial.print("[WIFI] Saving new SSID: ");
    Serial.println(ssid);


    server.send(200, "text/html", "Gespeichert. Neustart...");

    Serial.println("[WIFI] Credentials saved, restarting...");
    delay(1000);
    ESP.restart();
  });

  server.begin();
  Serial.println("[WIFI] Web server started");
}
