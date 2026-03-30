# ESP32-Based Smart Lighting System

## Abstract

This project presents the design and implementation of an intelligent lighting control system utilizing the ESP32 microcontroller platform. The system integrates multiple sensor modalities and communication protocols to provide autonomous and user-controlled lighting management. Three independent bulb control channels are implemented, each responding to distinct input mechanisms: infrared proximity detection, passive infrared motion sensing, and Bluetooth serial communication. A web-based graphical user interface enables centralized control via Wi-Fi connectivity, demonstrating practical integration of IoT principles in residential automation applications.

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

**Control Priority:** Sensor activation triggers automatic 60-second timeout. Manual control (Wi-Fi/Bluetooth) overrides sensor state and resets timers.

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

### Sensor Handling

**IR Proximity Detection (Bulb B1):**
- Active-LOW detection when object within 2-3 cm
- Activates B1, records timestamp; auto-deactivates after 60-second timeout

**PIR Motion Detection (Bulb B2):**
- Warm-up period: 30 seconds after power-on
- Activates B2, records timestamp; timer resets on continued motion detection
- Auto-deactivates after 60-second timeout with no motion

### Communication Protocols

**Bluetooth Serial (SPP):**
- Device name: `ESP32_SMART_HOME`
- Pairing PIN: `1234` (default)
- Commands: `B3ON` / `B3OFF` (newline-terminated, case-sensitive)

**Wi-Fi Web Server:**
- HTTP server on port 80
- Connects to configured Wi-Fi network
- Serves responsive HTML/CSS control interface

**HTTP Endpoints:**

| Endpoint | Action |
|----------|--------|
| `/B1ON` / `/B1OFF` | Control Bulb B1 |
| `/B2ON` / `/B2OFF` | Control Bulb B2 |
| `/B3ON` / `/B3OFF` | Control Bulb B3 |
| `/ALLON` / `/ALLOFF` | Control all bulbs |
| `/` | Serve control GUI |

### Display Management

LCD updates at 1 Hz showing real-time bulb status:

```
Display Format:
Line 1: IR  PIR  BT
Line 2: ON  OFF  ON
```

---

## User Interfaces

### Web-Based Control Panel

The system serves a responsive HTML5 interface with embedded CSS, providing individual bulb control and master ON/OFF switches.

<img src="Screenshots/GUI.jpg" alt="Web GUI Control Interface" width="300">

**Features:**
- Real-time color-coded status indicators
- Individual bulb ON/OFF buttons and master controls
- Mobile-responsive design; self-contained (no external dependencies)
- Dark theme with green (#22c55e) ON indicators, glow effects, Google Fonts (Rajdhani)

### Bluetooth Control Interface

Control via **Serial Bluetooth Terminal** (Kai Morich, available on Google Play Store).

<img src="Screenshots/BT_control.jpg" alt="Bluetooth Control Interface" width="250">

No Wi-Fi network infrastructure is required. See [Operational Usage](#operational-usage) for the full connection procedure.

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

---

## Setup and Installation

### Prerequisites

1. **Hardware Assembly:** Connect components according to [Pin Configuration](#pin-configuration)
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
3. Click Upload (Ctrl+U) and monitor Serial output at 115200 baud

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
3. Use individual bulb buttons or master controls (overrides sensor state)

**Manual Bluetooth Control:**
1. Pair Android device with `ESP32_SMART_HOME` (PIN: 1234)
2. Open Serial Bluetooth Terminal → connect to paired device
3. Send `B3ON` or `B3OFF` and observe confirmation messages

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

### System Performance

All primary objectives were successfully achieved. Three independent control mechanisms (IR, PIR, Bluetooth, Wi-Fi) operate without interference. The automatic 60-second timeout balances energy efficiency with user convenience.

### Practical Applications

- **Hallway/Corridor (B1 - IR):** Automatic activation when approaching doorway
- **Bathroom/Garage (B2 - PIR):** Motion-based lighting with automatic shutoff
- **Bedroom/Living Room (B3 - Manual):** Full manual control via web/Bluetooth

### Design Considerations

**Current Implementation:** Blocking web server (acceptable for single-user scenarios). Global boolean state management provides simple and effective tracking. Security considerations — plaintext Wi-Fi credentials and default Bluetooth PIN — are suitable for educational/personal use.

**Advantages Over Commercial Solutions:**
- Open-source, fully customizable, no cloud dependency or subscription fees
- Multi-protocol flexibility (Wi-Fi, Bluetooth, sensors)
- High educational value for IoT learning

---

## Conclusion

This project demonstrates a functional ESP32-based smart lighting system that effectively balances autonomous sensor-driven operation with manual control flexibility. The modular software architecture and comprehensive component testing methodology provide a robust foundation for residential automation applications, while the open-source codebase enables further customization for specific deployment requirements.

---

## License

This project is licensed under the **MIT License**.
See [LICENSE](LICENSE) file for complete terms.

---

**Project Repository:** https://github.com/Shass27/A_Smart_Bulb_System

**Author:** Shaswath S

**Last Updated:** March 2026
