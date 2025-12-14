# ❤️ Time2Love — Connected Countdown Device

**Time2Love** is a small, WiFi-connected countdown device built with an ESP8266 and an OLED display.  
It shows the remaining time until a meaningful event (e.g. the next time two people see each other) and allows two paired devices to communicate through a simple server — including a remote “blink” signal.

The device is designed to be housed in a **3D-printed enclosure** and supports **OTA firmware updates**.

---

## TO-DOs:

- Make ther server use NEXT.js
- use Websockets for instant blink responses
- Devices send heartbeats to track online status from the dashboard
- latency charts on dashboard
- custom OTA server
- remote configuration (and management) of the devices 

---

## ✨ Features

- ⏳ Countdown to a fixed event date (weeks / days / hours)
- 📡 WiFi-connected (ESP8266)
- 🖥 128×32 OLED display (SSD1306, U8g2)
- 🔘 Physical button to trigger a remote blink
- 💓 Remote blink when the paired device presses its button
- ☁️ Simple Node.js server for pairing & messaging
- 💾 Persistent configuration via EEPROM
- 🔄 OTA firmware updates via **OTAdrive**
- 🧪 Serial debug output for development

---

## 🧱 Hardware Requirements

- Wemos D1 Mini (ESP8266) or compatible clone
- 128×32 OLED display (SSD1306, I2C)
- Push button
- Optional LED (for blink indication)
- WiFi network
- 3D-printed enclosure

---

## 📁 Project Structure

```
├── esp_client/                         # ESP8266 firmware (Wemos D1 Mini)
│   ├── build/                          # Arduino build output (auto-generated)
│   │   └── esp8266.esp8266_d1_mini_clone/
│   │       ├── esp_client.ino.elf      # ELF binary (debug symbols)
│   │       ├── esp_client.ino.map      # Memory map file
│   │       └── firmware.bin            # Final firmware image (used for OTA)
│   │
│   ├── esp_client.ino                  # Main Arduino entry point (setup & loop)
│   ├── Config.h                        # Global configuration (WiFi, server URL, OTAdrive API key)
│   │
│   ├── ButtonHandler.h                 # Button input interface and debouncing logic
│   ├── ButtonHandler.cpp               # Button press handling and event triggering
│   │
│   ├── DisplayManager.h                # OLED display interface (SSD1306, U8g2)
│   ├── DisplayManager.cpp              # Countdown rendering, icons, and blink visualization
│   │
│   ├── EEPROMManager.h                 # EEPROM layout and persistent configuration interface
│   ├── EEPROMManager.cpp               # EEPROM initialization, read/write logic, defaults handling
│   │
│   ├── EventClock.h                    # Event countdown calculation interface
│   ├── EventClock.cpp                  # Time calculation (weeks / days / hours) and server sync
│   │
│   ├── ServerClient.h                  # HTTP client interface for server communication
│   ├── ServerClient.cpp                # Device registration, peer handling, blink messaging
│   │
│   ├── OTAManager.h                    # OTA update interface (OTAdrive)
│   └── OTAManager.cpp                  # OTA initialization, periodic update checks, reboot handling
│
├── server/                             # Simple Node.js backend server
│   ├── server.js                       # Express server (event date, device pairing, blink events)
│   ├── package.json                    # Node.js dependencies and scripts
│   └── package-lock.json               # Locked dependency versions
│
└── README.md                           # Project documentation
```

---

## 🧠 Architecture Overview

### ESP Device
- Connects to WiFi
- Registers itself at the server using a persistent `DEVICE_ID`
- Fetches a fixed event timestamp from the server
- Displays remaining time
- Polls for blink events
- Sends blink triggers on button press
- Periodically checks for OTA updates

### Server
- Keeps an in-memory list of registered devices
- Returns peer devices
- Stores blink events temporarily
- Provides a hard-coded event date

---

## 💾 EEPROM Usage

The EEPROM is used for **persistent configuration**:

| Key        | Description                    |
|-----------|--------------------------------|
| Device ID | Unique identifier per device   |
| Version   | Current firmware version       |

EEPROM is initialized on first boot and reused on every restart.

---

## 🔄 OTA Firmware Updates

OTA updates are handled using **OTAdrive**.

### Requirements
- OTAdrive account
- API key
- Internet access

### Behavior
- Device reports its `DEVICE_ID` and `SW_VERSION`
- Checks periodically for new firmware
- Automatically downloads and applies updates
- Reboots after successful update

---

## ⚙️ Configuration

Edit `Config.h`:

```cpp
#define WIFI_SSID "your-ssid"
#define WIFI_PASSWORD "your-password"

#define SERVER_URL "http://192.168.0.10:3000"

#define OTADRIVE_APIKEY "your-otadrive-api-key"