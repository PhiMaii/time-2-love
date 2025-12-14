// ================= WiFiManager.cpp =================
#include "WiFiManager.h"
#include "EEPROMManager.h"
#include "Config.h"

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
  Serial.println("[WIFI] Starting own AP ...");
  WiFi.mode(WIFI_AP);
  WiFi.softAP("Time2Love-SETUP");

  server.on("/", [this]() {
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

    EEPROMManager::setSsid(ssid);
    EEPROMManager::setWiFiPassword(pass);

    server.send(200, "text/html",
                "Gespeichert. Neustart...");

    delay(1000);
    // ESP.restart();
  });

  server.begin();
  Serial.println(String(WIFI_PREFIX) + "AP gestartet: Time2Love-SETUP");
}