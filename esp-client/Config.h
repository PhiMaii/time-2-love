#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials
#define WIFI_SSID "FRITZ!Box 7590 NU"
#define WIFI_PASS "95925916586060820281"

// Device ID - change on the second device
#define DEVICE_ID "time2love-01"

// Server URL (http)
#define SERVER_URL "http://192.168.178.153:3000" // change to your server IP:port

// Pins
#define BUTTON_PIN D5        // choose a safe pin (avoid pins that break boot)
#define LED_PIN LED_BUILTIN  // onboard LED

// Intervals (ms)
#define EVENT_FETCH_INTERVAL 10000UL  // 60s
#define BLINK_POLL_INTERVAL 1000UL    // 1s
#define REGISTER_INTERVAL 300000UL    // 5min

#endif