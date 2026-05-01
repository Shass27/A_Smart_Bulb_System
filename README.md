# ESP32-Based Smart Lighting System

## Abstract

This project presents an intelligent lighting control system built on the ESP32 microcontroller. Three independent channels (B1, B2, B3) are managed through IR proximity detection, PIR motion sensing, and manual control via Bluetooth or a Wi-Fi web interface — demonstrating practical IoT integration for residential automation.

---

## Table of Contents

1. [System Architecture](#system-architecture)
2. [Hardware Components](#hardware-components)
3. [Software Architecture](#software-architecture)
4. [Implementation Details](#implementation-details)
5. [User Interfaces](#user-interfaces)
6. [Testing and Validation](#testing-and-validation)
7. [Setup and Installation](#setup-and-installation)
8. [Results and Discussion](#results-and-discussion)
9. [Conclusion](#conclusion)
10. [License](#license)

---

## System Architecture

The system follows a four-layer hierarchy — User Interface → Communication → Processing (ESP32) → Actuators — with three independent control channels:

| Channel | Control Method | Sensor | GPIO (In/Out) | Auto-Timeout |
|---------|---------------|--------|---------------|--------------|
| B1 | IR Proximity + Manual | IR Obstacle | GPIO 15 / GPIO 4 | 60 s |
| B2 | PIR Motion + Manual | HC-SR501 | GPIO 27 / GPIO 26 | 60 s |
| B3 | Manual Only | — | — / GPIO 25 | None |

**Control priority:** Sensor activation starts a 60-second auto-off timer. Manual commands (Wi-Fi or Bluetooth) override sensor state and reset timers.

---

## Hardware Components

### Bill of Materials

| Component | Specification | Qty | Purpose |
|-----------|--------------|-----|---------|
| ESP32 Dev Board | Dual-core Xtensa LX6, 240 MHz, Wi-Fi + BT | 1 | Main MCU |
| IR Obstacle Sensor | 2–30 cm range, digital output | 1 | Proximity (B1) |
| PIR Motion Sensor | HC-SR501, 7 m range, 120° FOV | 1 | Motion (B2) |
| 1-Ch Relay Module | 5 V trigger, 10 A/250 VAC | 3 | AC bulb switching |
| 16×2 I2C LCD | PCF8574 backpack, address 0x27 | 1 | Status display |
| AC Bulbs | 230 V/110 V, E27 socket | 3 | Lighting outputs |
| Power Supply | 5 V/2 A | 1 | System power |

### Pin Configuration

```
ESP32 GPIO Mapping:
├── Sensor Inputs
│   ├── GPIO 15  →  IR Sensor
│   └── GPIO 27  →  PIR Sensor
│
├── Relay Outputs (Active-LOW; initialized HIGH at boot to prevent spurious activation)
│   ├── GPIO 4   →  Bulb B1
│   ├── GPIO 26  →  Bulb B2
│   └── GPIO 25  →  Bulb B3
│
└── I2C (LCD)
    ├── GPIO 21  →  SDA
    └── GPIO 22  →  SCL
```

**Notes:** IR sensor provides a clean digital output (no pull resistor needed). PIR is configured with `INPUT_PULLUP` for noise immunity and requires a 30-second warm-up after power-on. Relays provide galvanic isolation between the 5 V control circuit and the 230 V AC load.

---

## Software Architecture

### Development Environment

- **Platform:** Arduino IDE 2.x with ESP32 Arduino Core v2.x
- **Language:** C++

### Dependencies

| Library | Source | Purpose |
|---------|--------|---------|
| `WiFi.h` | ESP32 built-in | Wi-Fi and HTTP server |
| `BluetoothSerial.h` | ESP32 built-in | Bluetooth Classic SPP |
| `Wire.h` | ESP32 built-in | I2C for LCD |
| `LiquidCrystal_I2C.h` | Frank de Brabander | LCD control |

### Code Structure

```
Final_Code/
├── Final_Code.ino      # setup() and loop()
├── functions.h         # Sensor and timer logic
└── wifi_handling.h     # HTTP server and web GUI
```

---

## Implementation Details

### Main Loop

Non-blocking, event-driven architecture:

```cpp
void loop() {
  handleIR();       // Poll IR; activate B1 if triggered
  handlePIR();      // Poll PIR; activate B2 if triggered
  handleBluetooth();// Parse incoming BT commands
  handleWiFi();     // Service HTTP requests
  handleTimers();   // Auto-timeout for B1/B2
  updateLCD();      // Refresh at 1 Hz
}
```

### Communication Protocols

**Bluetooth Serial (SPP)**
- Device name: `ESP32_SMART_HOME` | PIN: `1234`
- Commands: `B3ON` / `B3OFF` (newline-terminated, case-sensitive)

**Wi-Fi Web Server** — HTTP on port 80

| Endpoint | Action |
|----------|--------|
| `/B1ON` `/B1OFF` | Bulb B1 |
| `/B2ON` `/B2OFF` | Bulb B2 |
| `/B3ON` `/B3OFF` | Bulb B3 |
| `/ALLON` `/ALLOFF` | All bulbs |
| `/` | Serve control GUI |

### Display

LCD updates at 1 Hz:
```
Line 1: IR  PIR  BT
Line 2: ON  OFF  ON
```

---

## User Interfaces

### Web Control Panel

Responsive HTML5 interface served directly by the ESP32 — no external dependencies. Features color-coded status indicators, individual and master ON/OFF controls, dark theme with green (`#22c55e`) active state, and mobile-responsive layout.

<img src="Screenshots/GUI.jpg" alt="Web GUI" width="300">

### Bluetooth Interface

Controlled via **Serial Bluetooth Terminal** (Kai Morich, Google Play). No Wi-Fi infrastructure required. See [Operational Usage](#operational-usage).

<img src="Screenshots/BT_control.jpg" alt="BT Interface" width="250">

---

## Testing and Validation

Testing follows an incremental four-stage approach using isolated sketches in `Components_Testing/`:

1. **Hardware Verification** — GPIO config, Serial output, I2C bus scan, LCD rendering
2. **Sensor Characterization** — IR potentiometer calibration, PIR 30-second warm-up, relay active-LOW validation
3. **Communication Protocols** — Bluetooth pairing and echo test, Wi-Fi DHCP and HTTP handling
4. **Integration** — Multi-channel simultaneous operation, concurrent timer behavior, LCD refresh rate

---

## Setup and Installation

### Prerequisites

- Arduino IDE 2.x
- ESP32 board support URL: `https://dl.espressif.com/dl/package_esp32_index.json`
- LiquidCrystal_I2C (via Library Manager)

### Arduino IDE Settings

```
Board:            ESP32 Dev Module
Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)  ← MANDATORY
Upload Speed:     115200
```

> ⚠️ The default partition causes a "text section exceeds available space" error. **Huge APP is required.**

### Wi-Fi Configuration

Edit lines 17–18 of `Final_Code.ino`:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

> Remove credentials before pushing to version control.

### Expected Startup Output

**Serial Monitor (115200 baud):**
```
Bluetooth Ready
WiFi Connected
IP address: 192.168.x.x
```

**LCD sequence:** `Smart Bulb System` → `Connecting WiFi` → `WiFi Connected / <IP>` → operational status

### Operational Usage

| Method | Steps |
|--------|-------|
| IR (B1) | Approach sensor within 3 cm → activates for 60 s; resets on continued detection |
| PIR (B2) | Enter 7 m / 120° zone → activates for 60 s; resets on motion |
| Web | Connect to same Wi-Fi → navigate to LCD-displayed IP |
| Bluetooth | Pair with `ESP32_SMART_HOME` (PIN: 1234) → send `B3ON` / `B3OFF` |

### Troubleshooting

| Symptom | Cause | Fix |
|---------|-------|-----|
| "text section exceeds space" | Wrong partition | Set Huge APP |
| LCD blank | Wrong I2C address | Run `Identifying_I2C_address.ino` |
| Wi-Fi fails | Wrong credentials or 5 GHz band | Verify SSID/password; use 2.4 GHz |
| BT not discoverable | Sketch not running | Confirm "Bluetooth Ready" in Serial |
| Relay clicks, bulb off | Wiring issue | Check relay COM/NO terminals |
| PIR false triggers | Warm-up incomplete | Wait 30 s after power-on |

---

## Results and Discussion

### Build Result

<img src="What_I_Built.HEIC" alt="Build Result" width="600">

All objectives were met. The three control modes (IR, PIR, Bluetooth/Wi-Fi) operate without interference, and the 60-second timeout balances energy efficiency with usability.

**Practical use cases:**
- **B1 (IR):** Hallway — auto-on when approaching a doorway
- **B2 (PIR):** Bathroom/garage — motion-activated with auto-shutoff
- **B3 (Manual):** Bedroom/living room — full user control

**Design notes:** The web server is currently blocking (acceptable for single-user use). Wi-Fi credentials and the default Bluetooth PIN are plaintext — suitable for personal/educational use, not production deployment.

---

## Conclusion

This project demonstrates a functional, modular ESP32 smart lighting system that balances autonomous sensor operation with flexible manual control. Its open-source, cloud-free design makes it practical for residential use and adaptable for further development.

---

## License

**MIT License** — see [LICENSE](LICENSE) for full terms.

---

**Repository:** https://github.com/Shass27/A_Smart_Bulb_System  
**Author:** Shaswath S  
**Last Updated:** March 2026
