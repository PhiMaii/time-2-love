// WiFiManager.cpp

#include "ESP8266WiFi.h"
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
  if (WiFi.status() != WL_CONNECTED) server.handleClient();
  // server.handleClient();
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

  // server.on("/", [this]() {
  //   Serial.println("[WIFI] Client connected to captive portal");
  //   server.send(200, "text/html",
  //               "<h2>WiFi Setup</h2>"
  //               "<form action='/save'>"
  //               "SSID:<br><input name='s'><br>"
  //               "Passwort:<br><input name='p' type='password'><br><br>"
  //               "<button>Speichern</button>"
  //               "</form>");
  // });

  server.on("/", [this]() {
    Serial.println("[WIFI] Client connected to captive portal");
    server.send(200, "text/html",
                "<!DOCTYPE html>"
                "<html><head>"
                "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                "<title>WiFi Setup</title>"
                "<style>"
                "*{margin:0;padding:0;box-sizing:border-box;font-family:system-ui,-apple-system,sans-serif}"
                "body{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;"
                "display:flex;align-items:center;justify-content:center;padding:20px}"
                ".card{background:#fff;border-radius:20px;padding:40px;max-width:400px;width:100%;"
                "box-shadow:0 20px 40px rgba(0,0,0,0.1);text-align:center}"
                "h1{color:#333;font-size:28px;margin-bottom:10px}"
                "p{color:#666;margin-bottom:30px;font-size:16px}"
                "input{width:100%;padding:15px;border:2px solid #e1e5e9;border-radius:12px;"
                "font-size:16px;margin-bottom:20px;transition:all 0.3s}"
                "input:focus{outline:none;border-color:#667eea;box-shadow:0 0 0 3px rgba(102,126,234,0.1)}"
                "button{width:100%;padding:16px;background:linear-gradient(135deg,#667eea,#764ba2);"
                "color:white;border:none;border-radius:12px;font-size:16px;font-weight:600;"
                "cursor:pointer;transition:all 0.3s}"
                "button:hover{transform:translateY(-2px);box-shadow:0 10px 25px rgba(102,126,234,0.4)}"
                "button:active{transform:translateY(0)}"
                "@media (max-width:480px){.card{padding:30px 20px}}"
                "</style>"
                "</head><body>"
                "<div class='card'>"
                "<h1>WiFi Setup</h1>"
                "<p>SSID und Passwort eingeben</p>"
                "<form action='/save'>"
                "<input name='s' placeholder='z.B. MeinWLAN' required maxlength='32'>"
                "<input name='p' type='password' placeholder='Passwort' required maxlength='64'>"
                "<button>Speichern & Verbinden</button>"
                "</form>"
                "</div></body></html>");
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
