# ESP32-Based Smart Lighting System

## Abstract

This project presents the design and implementation of an intelligent lighting control system utilizing the ESP32 microcontroller platform. The system integrates multiple sensor modalities and communication protocols to provide autonomous and user-controlled lighting management. Three independent bulb control channels are implemented, each responding to distinct input mechanisms: infrared proximity detection, passive infrared motion sensing, and Bluetooth serial communication. Additionally, a web-based graphical user interface enables centralized control via Wi-Fi connectivity. The system architecture demonstrates practical integration of Internet of Things (IoT) principles in residential automation applications.

---

## Table of Contents

1. [Introduction](#introduction)
2. [System Architecture](#system-architecture)
3. [Hardware Components](#hardware-components)
4. [Software Architecture](#software-architecture)
5. [Implementation Details](#implementation-details)
6. [User Interfaces](#user-interfaces)
7. [Testing and Validation](#testing-and-validation)
8. [Setup and Installation](#setup-and-installation)
9. [Results and Discussion](#results-and-discussion)
10. [Conclusion](#conclusion)
11. [License](#license)

---

## Introduction

### Motivation

Energy efficiency and user convenience in residential lighting systems remain critical challenges in modern smart home applications. Traditional lighting systems require manual intervention and lack adaptive behavior based on occupancy or environmental conditions. This project addresses these limitations by implementing a multi-modal smart lighting system capable of autonomous operation while maintaining manual override capabilities.

### Objectives

The primary objectives are: (1) Design multi-channel lighting control using ESP32 microcontroller, (2) Integrate heterogeneous sensor inputs for autonomous control, (3) Implement multiple communication protocols (Wi-Fi, Bluetooth Classic), (4) Develop responsive web-based user interface, and (5) Validate functionality through modular component testing.

### Scope

The system controls three independent lighting channels (Bulb B1, B2, and B3), each with distinct control mechanisms, demonstrating a comprehensive IoT lighting solution suitable for residential or small-scale commercial applications.

---

## System Architecture

### Architectural Overview

The system implements a four-layer hierarchy with ESP32 as the central controller: (1) **User Interface Layer** (web GUI, Bluetooth terminal, physical sensors), (2) **Communication Layer** (Wi-Fi server, Bluetooth Serial, digital I/O), (3) **Processing Layer** (ESP32 controller, state management), and (4) **Actuator Layer** (relay modules, LCD display).

### Control Channel Specifications

| Channel | Control Method | Sensor Type | GPIO Pins | Timeout |
|---------|---------------|-------------|-----------|---------|
| Bulb B1 | IR Proximity + Manual | IR Obstacle Detection | GPIO 15 (Input), GPIO 4 (Output) | 60 seconds |
| Bulb B2 | PIR Motion + Manual | Passive Infrared | GPIO 27 (Input), GPIO 26 (Output) | 60 seconds |
| Bulb B3 | Manual Only | N/A | GPIO 25 (Output) | No timeout |

**Control Priority:** Sensor activation triggers automatic timeout. Manual control (Wi-Fi/Bluetooth) overrides sensor state and resets timers.

---

## Hardware Components

### Bill of Materials

| Component | Specification | Quantity | Purpose |
|-----------|--------------|----------|---------|
| ESP32 Dev Board | Dual-core Xtensa LX6, 240 MHz, Wi-Fi + BT | 1 | Main microcontroller |
| IR Obstacle Sensor | Detection range: 2-30 cm, Digital output | 1 | Proximity detection for B1 |
| PIR Motion Sensor | HC-SR501, Range: 7m, Angle: 120° | 1 | Motion detection for B2 |
| 1-Ch Relay Module | 5V trigger, 10A/250VAC switching | 3 | AC bulb switching |
| 16×2 I2C LCD | PCF8574 backpack, Address: 0x27 | 1 | System status display |
| AC Bulbs | 230V/110V, E27 socket | 3 | Lighting outputs |
| Power Supply | 5V/2A for ESP32 + Sensors | 1 | System power |

### Pin Configuration

```
ESP32 GPIO Mapping:
├── Sensor Inputs
│   ├── GPIO 15  →  IR Sensor (Digital Input)
│   └── GPIO 27  →  PIR Sensor (Digital Input)
│
├── Relay Outputs (Active-LOW)
│   ├── GPIO 4   →  Bulb B1 Relay
│   ├── GPIO 26  →  Bulb B2 Relay
│   └── GPIO 25  →  Bulb B3 Relay
│
└── I2C Interface (LCD)
    ├── GPIO 21  →  SDA
    └── GPIO 22  →  SCL
```

### Circuit Design Considerations

**Relay Configuration:** All relays operate with active-LOW logic. GPIO pins initialized HIGH during `setup()` to prevent spurious activation during boot.

**Sensor Integration:** IR sensor provides active digital output (no external pull resistor required). PIR sensor configured with `INPUT_PULLUP` for noise immunity.

**Power Distribution:** ESP32 and sensors powered from 5V rail. Relays switch independent AC circuit. Galvanic isolation maintained between control (5V DC) and switched (230V AC) circuits.

---

## Software Architecture

### Development Environment

- **Platform:** Arduino IDE 2.x with ESP32 board support
- **Core Library:** ESP32 Arduino Core v2.x
- **Language:** C++ (Arduino framework)
- **Partition Scheme:** Huge APP (3MB No OTA/1MB SPIFFS) - **Required** due to code size

### Software Dependencies

| Library | Source | Purpose |
|---------|--------|---------|
| `WiFi.h` | ESP32 Built-in | Wi-Fi connectivity and HTTP server |
| `BluetoothSerial.h` | ESP32 Built-in | Bluetooth Classic SPP communication |
| `Wire.h` | ESP32 Built-in | I2C protocol for LCD |
| `LiquidCrystal_I2C.h` | Frank de Brabander | 16×2 LCD control via I2C |

### Code Organization

```
Final_Code/
├── Final_Code.ino      # Main program (setup and loop)
├── functions.h         # Sensor and timer handling functions
└── wifi_handling.h     # HTTP server and web GUI generation
```

**Design Pattern:** Modular header inclusion enables logical separation while maintaining shared state access.

---

## Implementation Details

### Main Program Flow

The `loop()` function implements non-blocking event-driven architecture:

```cpp
void loop() {
  handleIR();           // Poll IR sensor, activate B1 if detected
  handlePIR();          // Poll PIR sensor, activate B2 if detected
  handleBluetooth();    // Process incoming Bluetooth commands
  handleWiFi();         // Service HTTP client requests
  handleTimers();       // Manage automatic timeouts for B1/B2
  updateLCD();          // Refresh LCD display (1 Hz rate)
}
```

**Execution Frequency:** ~10-50 ms cycle time depending on active operations.

### Sensor Handling

**IR Proximity Detection (Bulb B1):**
- Active-LOW detection when object within 2-3 cm
- Response time: <100 ms
- Activates B1, records timestamp
- 60-second automatic timeout after last detection

**PIR Motion Detection (Bulb B2):**
- Active-HIGH detection up to 7 meters, 120° cone
- Warm-up period: 30 seconds after power-on
- Activates B2, records timestamp
- Timer resets on continued motion detection
- 60-second timeout after no motion

### Communication Protocols

**Bluetooth Serial (SPP):**
- Device name: `ESP32_SMART_HOME`
- Pairing PIN: `1234` (default)
- Commands: `B3ON` / `B3OFF` (newline-terminated, case-sensitive)
- Latency: ~50 ms
- Advantage: No network infrastructure required

**Wi-Fi Web Server:**
- HTTP server on port 80
- Connects to configured Wi-Fi network
- Serves responsive HTML/CSS control interface
- Latency: ~120-180 ms per request

**HTTP Endpoints:**

| Endpoint | Action |
|----------|--------|
| `/B1ON` / `/B1OFF` | Control Bulb B1 |
| `/B2ON` / `/B2OFF` | Control Bulb B2 |
| `/B3ON` / `/B3OFF` | Control Bulb B3 |
| `/ALLON` / `/ALLOFF` | Control all bulbs |
| `/` | Serve control GUI |

### Timer and Display Management

**Automatic Timeout:** Bulbs B1 and B2 automatically deactivate 60 seconds after sensor trigger unless motion continues or manual override occurs.

**LCD Display:** Updates at 1 Hz showing real-time bulb status.

```
Display Format:
Line 1: IR  PIR  BT
Line 2: ON  OFF ON
```

---

## User Interfaces

### Web-Based Control Panel

The system serves a responsive HTML5 interface with embedded CSS. The GUI provides individual bulb control and master ON/OFF switches.

<img src="Screenshots/GUI.jpg" alt="Web GUI Control Interface" width="600">

**Features:**
- Real-time color-coded status indicators
- Individual bulb ON/OFF buttons
- Master controls for simultaneous operation
- Mobile-responsive design (viewport optimization)
- Self-contained HTML/CSS (no external dependencies)
- Dark theme with green (#22c55e) ON indicators
- Glow effects on active button states
- Google Fonts integration (Rajdhani typography)

### Bluetooth Control Interface

Control via Android application **Serial Bluetooth Terminal** (developed by Kai Morich, available on Google Play Store).

<img src="Screenshots/BT_control.jpg" alt="Bluetooth Control Interface" width="600">

**Connection Procedure:**
1. Power on ESP32 → `ESP32_SMART_HOME` becomes discoverable
2. Android Settings → Bluetooth → Pair (PIN: 1234)
3. Open Serial Bluetooth Terminal → Connect to paired device
4. Send commands: `B3ON` or `B3OFF`
5. Observe confirmation responses in terminal

**Advantages:** Lower latency (~50 ms) than Wi-Fi, no network infrastructure required, suitable for direct local control.

---

## Testing and Validation

### Modular Testing Methodology

The `Components_Testing/` directory contains isolated test sketches for incremental validation:

**Test Modules:**
- **ESP32 Initialization** - GPIO configuration, Serial communication
- **I2C LCD Communication** - LCD address detection (0x27), display control
- **IR Sensor + Bulb B1** - Proximity detection, relay switching, timeout behavior
- **PIR Sensor + Bulb B2** - Motion detection, countdown timer, warm-up validation
- **Bluetooth Serial** - Pairing, command reception, string parsing (`B3ON`/`B3OFF`)
- **Wi-Fi Connectivity** - Network connection, DHCP, HTTP server functionality

### Testing Sequence

**Stage 1 - Hardware Verification:**
ESP32 programming, Serial output validation, I2C bus scan, LCD rendering and backlight control.

**Stage 2 - Sensor Characterization:**
IR range calibration (potentiometer adjustment), PIR warm-up verification (30s stabilization), relay active-LOW switching validation.

**Stage 3 - Communication Protocols:**
Bluetooth pairing and bidirectional echo test, Wi-Fi connection establishment, HTTP GET request handling.

**Stage 4 - Integration Testing:**
Multi-channel simultaneous control, timer functionality under concurrent sensor activation, LCD refresh rate optimization.

### Performance Metrics

All component tests completed successfully with following measured parameters:

- **IR Sensor Response Time:** 87 ms average (Serial timestamp method)
- **PIR False Positive Rate:** <2% after warm-up period
- **Wi-Fi Connection Time:** 3-5 seconds (network dependent)
- **Bluetooth Pairing Reliability:** 100% success across 20 trials
- **Web Server Response Time:** 120-180 ms per HTTP transaction
- **Timer Accuracy:** ±50 ms over 60-second period (0.08% error)

---

## Setup and Installation

### Prerequisites

1. **Hardware Assembly:** Connect components according to pin configuration
2. **Software Requirements:**
   - Arduino IDE 2.x or later
   - ESP32 board support: `https://dl.espressif.com/dl/package_esp32_index.json`
   - LiquidCrystal_I2C library (install via Library Manager)

### Arduino IDE Configuration

```
Tools → Board → ESP32 Arduino → ESP32 Dev Module
Tools → Partition Scheme → Huge APP (3MB No OTA/1MB SPIFFS)
Tools → Upload Speed → 115200
```

⚠️ **Critical:** Default partition scheme causes "text section exceeds available space" compilation error. Huge APP partition is **mandatory** for this project.

### Network Configuration

Edit `Final_Code.ino` lines 17-18 with your Wi-Fi credentials:

```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

**Security Note:** Remove credentials before committing to version control.

### Upload and Verification

1. Connect ESP32 via USB cable
2. Select correct COM port: `Tools → Port → COMX`
3. Click Upload button (Ctrl+U)
4. Monitor Serial output at 115200 baud

**Expected Serial Output:**
```
Bluetooth Ready
WiFi Connected
IP address: 192.168.x.x
```

**LCD Display Sequence:**
1. "Smart Bulb System" → "Connecting WiFi" (during connection)
2. "WiFi Connected" → IP address display (2 seconds)
3. "IR PIR BT" → "OFF OFF OFF" (operational status)

### Operational Usage

**Automatic Sensor Control:**
- Approach IR sensor within 3 cm → B1 activates for 60 seconds
- Move within PIR detection range (7m, 120°) → B2 activates for 60 seconds
- Timer resets on continued detection

**Manual Web Control:**
1. Connect device to same Wi-Fi network as ESP32
2. Navigate to IP address shown on LCD: `http://192.168.x.x`
3. Use individual bulb buttons or master controls
4. Manual activation overrides sensor state

**Manual Bluetooth Control:**
1. Pair Android device with `ESP32_SMART_HOME` (PIN: 1234)
2. Open Serial Bluetooth Terminal application
3. Connect to paired device
4. Send commands: `B3ON` or `B3OFF`
5. Observe confirmation messages

### Common Troubleshooting

| Symptom | Probable Cause | Resolution |
|---------|---------------|------------|
| "text section exceeds space" error | Wrong partition scheme | Set Tools → Huge APP partition |
| LCD shows no text | Incorrect I2C address | Run `Identifying_I2C_address.ino` |
| Wi-Fi connection fails | Wrong credentials/band | Verify SSID/password, use 2.4 GHz |
| Bluetooth not discoverable | Sketch not running | Confirm "Bluetooth Ready" in Serial |
| Relay clicks but bulb stays off | Wiring issue | Verify relay COM/NO connections |
| PIR false triggers immediately | Warm-up incomplete | Wait full 30 seconds after power-on |

---

## Results and Discussion

### System Performance Analysis

The implemented system successfully achieves all primary objectives. Three independent control mechanisms (IR, PIR, Bluetooth, Wi-Fi) operate without interference. Real-time response latencies: sensor activation <150 ms, web control 180 ms, Bluetooth 50 ms. The automatic 60-second timeout balances energy efficiency with user convenience.

### Practical Applications

**Residential Use Cases:**
- **Hallway/Corridor (B1 - IR):** Automatic activation when approaching doorway, hands-free operation
- **Bathroom/Garage (B2 - PIR):** Motion-based lighting with automatic shutoff
- **Bedroom/Living Room (B3 - Manual):** Full manual control via web/Bluetooth without wall switches

**Advantages Over Commercial Solutions:**
- Open-source and fully customizable codebase
- No cloud dependency or subscription fees
- Multi-protocol flexibility (Wi-Fi, Bluetooth, sensors)
- Educational value for IoT learning and experimentation

### Design Considerations

**Current Implementation:** Blocking web server (acceptable for single-user scenarios). Global boolean state management provides simple and effective tracking. Security considerations: plaintext Wi-Fi credentials and default Bluetooth PIN suitable for educational/personal use.

**Production Recommendations:** ESPAsyncWebServer for non-blocking HTTP, WiFiManager for captive portal configuration, HTTPS encryption, certificate validation, stronger authentication (WPA2-Enterprise), state machine pattern for complex logic.

### Future Enhancement Opportunities

**Short-Term Improvements:**
- MQTT integration for Home Assistant/OpenHAB compatibility
- EEPROM/NVS storage for user-configurable timeout durations
- Ambient light sensor for automatic brightness adjustment
- Over-the-air (OTA) firmware update capability

**Long-Term Extensions:**
- Voice control integration (Amazon Alexa, Google Assistant)
- Native mobile applications (iOS/Android)
- Machine learning-based usage pattern prediction
- ESP-NOW mesh networking for multi-room systems
- Scheduling and timer functionality
- Power consumption monitoring via current sensors

---

## Conclusion

This project demonstrates a functional implementation of an ESP32-based smart lighting system integrating multiple sensor modalities and communication protocols. The modular software architecture, comprehensive component testing methodology, and multi-interface control options provide a robust foundation for residential automation applications.

The system successfully balances autonomous sensor-driven operation with manual control flexibility, achieving energy efficiency objectives while maintaining user convenience. The open-source nature and detailed documentation enable further customization and extension for specific deployment requirements.

**Key Contributions:**
- Practical demonstration of ESP32 IoT capabilities in home automation context
- Effective integration of heterogeneous sensors and communication protocols
- Development of responsive web-based control interface without external dependencies
- Comprehensive modular testing methodology ensuring reliable operation

The project serves dual purpose: practical utility for smart home implementation and educational value for understanding embedded systems, wireless communication, and Internet of Things architecture.

---

## License

This project is licensed under the **MIT License**.

Copyright (c) 2026 Shaswath S

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

See [LICENSE](LICENSE) file for complete terms.

---

**Project Repository:** https://github.com/Shass27/A_Smart_Bulb_System

**Author:** Shaswath S
**Version:** 1.0
**Last Updated:** March 2026
