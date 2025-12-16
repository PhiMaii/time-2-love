
[![Deploy server](https://github.com/PhiMaii/time-2-love/actions/workflows/deploy.yml/badge.svg)](https://github.com/PhiMaii/time-2-love/actions/workflows/deploy.yml) [![Build  firmware](https://github.com/PhiMaii/time-2-love/actions/workflows/build.yml/badge.svg)](https://github.com/PhiMaii/time-2-love/actions/workflows/build.yml)

---

# ❤️ Time2Love — Connected Countdown Device

**Time2Love** is a small, WiFi-connected countdown device built with an ESP8266 and an OLED display.  
It shows the remaining time until a meaningful event (e.g. the next time two people see each other) and allows two paired devices to communicate through a simple server — including a remote “blink” signal.

---

## TO-DOs

- [ ] Server/Backend improvements:
  - [ ] Make a more fancy server using NEXT.js
  - [ ] Implement a custom DIY OTA server
  - [ ] latency and uptime/online charts on dashboard
  
- [ ] Hardware improvements
  - [ ] ...
  
- [ ] Future feature ideas:
  - [ ] use Websockets for instant blink responses
  - [ ] Devices send heartbeats to track online status from the dashboard
  - [ ] remote configuration (and management) of the devices 


---

## Key features

- Countdown to a fixed event date (weeks / days / hours / minutes)
- WiFi-connected event synching
- 128×32 OLED display (SSD1306, U8g2)
- 2 physical buttons to trigger a remote blink and toggle sleep
- Remote blink when the paired device presses its button
- Simple Node.js server for pairing & messaging
- Persistent configuration via **EEPROM**
- OTA firmware updates via **OTAdrive**

---

## Required hardware

- Wemos D1 Mini (ESP8266) or compatible clone
- 128×32 OLED display (SSD1306, I2C)
- 2 Push buttons
- WS2812B adressable RGB LEDs
- (Optional: 3D printed case) 

---

## 📁 Project Structure

```
├── esp_client/                         # ESP8266 firmware (Wemos D1 Mini)
│   ├── build/                          # Arduino build output (auto-generated)
│   │   └── esp8266.esp8266_d1_mini_clone/
│   │       ├── esp_client.ino.elf      # 
│   │       ├── esp_client.ino.map      # 
│   │       └── firmware.bin            # Final firmware image (used for OTA)
│   │
│   ├── esp_client.ino                  # Main Arduino entry point (setup & loop)
│   ├── Config.h                        # Global configuration (WiFi, server URL, OTAdrive API key)
│   │
│   ├── ButtonHandler.h                 # Button press and hold handling and event triggering
│   ├── ButtonHandler.cpp               # 
│   │
│   ├── DisplayManager.h                # Display rendering, icons, countdown and blink visualization
│   ├── DisplayManager.cpp              # 
│   │
│   ├── EEPROMManager.h                 # EEPROM initialization, read/write logic, defaults handling
│   ├── EEPROMManager.cpp               # 
│   │
│   ├── EventClock.h                    # Time calculation (weeks / days / hours / minutes) and server sync
│   ├── EventClock.cpp                  # 
│   │
│   ├── WiFiManager.h                   # WiFi setup and connection
│   ├── WiFiManager.cpp                 #
│   │
│   ├── ServerClient.h                  # Device registration, peer handling, blink messaging via HTTP
│   ├── ServerClient.cpp                #
│   │
│   ├── LEDManager.h                    # Initialization and controlling of the LEDs
│   ├── LEDManager.cpp                  #
│   │
│   ├── BootManager.h                   # Startup sequence and display boot progress
│   ├── BootManager.cpp                 #
│   │
│   ├── DelayedCaller.h                 # Helper to delay a function call by a variable amount
│   ├── DelayedCaller.cpp               #
│   │
│   ├── OTAManager.h                    # OTA initialization, periodic update checks, reboot handling
│   └── OTAManager.cpp                  # 
│
├── server/                             # Simple Node.js backend server
│   ├── Dockerfile                      # Creates the docker container thats used to run the server
│   ├── entrypoint.sh                   # Used by the local GitHub runner to pull the latest changes from GitHub
│   ├── server.js                       # Express server handles: event date, device pairing, blink events
│   ├── package.json                    # Node.js dependencies and scripts
│   └── package-lock.json               # Locked dependency versions
│
└── README.md                           # Project documentation
```

---

## Architecture Overview

### ESP Device
- Connects to WiFi
- Registers itself at the server using a persistent `DEVICE_ID`
- Fetches a fixed event timestamp from the server
- Checks for newer firmware versions
- Displays remaining time to the event
- Polls for blink events periodically
- Sends blink triggers on button press

### Server
- Keeps an in-memory list of registered devices
- Returns peer devices
- Stores blink events temporarily
- Provides a hard-coded event date

---

## 💾 EEPROM Usage

The EEPROM is used for **persistent configuration**:

| Name        | Description                    | Start Addr | Length |
|-----------|--------------------------------|---------|-----|
| Magic bit | Shows if the EEPROM has been written to before   |0|1|
| Device ID   | Unique device ID       |1|32|
|SF Version| The current firmware version of the device|33|32|
|SSID|SSID of a stored WiFi network | 65| 32|
|WiFi Password| Password for the stored WiFi network | 97|64|

EEPROM is initialized on first boot and reused on every restart.
Total EEPROM size is 256.

---
