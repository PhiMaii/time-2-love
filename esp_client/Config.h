#ifndef CONFIG_H
#define CONFIG_H

#define SOFTWARE_VERSION "0.1.0"

// WiFi credentials
#define WIFI_SSID "FRITZ!Box 7590 NU"
#define WIFI_PASS "95925916586060820281"

// Device ID - change on the second device
// #define DEVICE_ID "time2love-01"

// Server URL (http)
#define SERVER_URL "http://192.168.178.153:3000" // change to your server IP:port

// Pins
#define BUTTON_PIN D5        // choose a safe pin (avoid pins that break boot)
#define LED_PIN LED_BUILTIN  // onboard LED

// Intervals (ms)
#define EVENT_FETCH_INTERVAL 60UL * 1000UL  // 60s
#define BLINK_POLL_INTERVAL 1UL * 1000UL    // 1s
#define REGISTER_INTERVAL 5UL * 60UL * 1000UL    // 5min

#define OTA_CHECK_INTERVAL 1UL * 60UL * 60UL * 1000UL // 1 h

#endif