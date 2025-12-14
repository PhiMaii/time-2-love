#ifndef CONFIG_H
#define CONFIG_H

// ===== Device Infos =====
// #define SOFTWARE_VERSION "0.1.2" -> just used for Github actions - Moved to EEPROM
// #define DEVICE_ID "time2love-01" -> Moved to EEPROM

// ===== Debug =====
#define DEBUG_MODE true

#define DISPLAY_PREFIX "[DISPLAY] "
#define SERVER_PREFIX "[SERVER] "
#define EEPROM_PREFIX "[EEPROM] "
#define OTA_PREFIX "[OTA] "
#define WIFI_PREFIX "[WIFI] "

// ===== WiFi =====
#define WIFI_SSID "FRITZ!Box 7590 NU"
#define WIFI_PASS "95925916586060820281"

// ===== Credentials ====
#define OTA_DRIVE_API_KEY "c51d0a1f-1c63-4e69-862b-9de2445162ff"

// ===== Server (http) =====
#define SERVER_URL "http://192.168.178.153:3000"  // change to your server IP:port

// ===== Pins =====
#define BUTTON_PIN D5        //  a safe pin
#define LED_PIN LED_BUILTIN  // onboard LED

// ===== Intervals (ms) =====
#define EVENT_FETCH_INTERVAL 60UL * 1000UL     // 60s
#define BLINK_POLL_INTERVAL 1UL * 1000UL       // 1s
#define REGISTER_INTERVAL 5UL * 60UL * 1000UL  // 5min

#define OTA_CHECK_INTERVAL 12UL * 60UL * 60UL * 1000UL  // 12 h

#define DEBUG_INTERVAL 10UL * 1000UL  // 5s

// ===== EEPROM Layout =====
#define EEPROM_SIZE 256
#define EEPROM_MAGIC_ADDR 0
#define EEPROM_MAGIC_VAL 0x42

// Device id
#define EEPROM_DEVICE_ID_ADDR 1
#define EEPROM_DEVICE_ID_LEN 32

// Firmware version
#define EEPROM_SW_VERSION_ADDR 33
#define EEPROM_SW_VERSION_LEN 32

// WiFi SSID
#define EEPROM_SSID_ADDR 65
#define EEPROM_SSID_LEN 32

// WiFi Password
#define EEPROM_WIFI_PASSWORD_ADDR 97
#define EEPROM_WIFI_PASSWORD_LEN 64


#endif