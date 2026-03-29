# Bluetooth Communication with ESP32 — Setup and Usage Guide

**Project:** ESP32-Based Smart Lighting System
**Module:** Bulb B3 — Bluetooth Serial Control
**Device Name:** `Shas_ESP32` | **Control Pin:** GPIO 25 (Relay, Active-LOW)

---

## 1. Overview

The ESP32 microcontroller integrates a dual-mode Bluetooth radio supporting both Classic Bluetooth (BR/EDR) and Bluetooth Low Energy (BLE). This module employs **Classic Bluetooth with the Serial Port Profile (SPP)**, which emulates a standard UART serial channel over a wireless link. From the application perspective, Bluetooth serial communication is functionally identical to wired USB serial communication — the same read/write primitives apply; only the physical medium differs.

The `BluetoothSerial` library, bundled with the ESP32 Arduino core, abstracts the underlying Bluetooth stack and exposes an interface mirroring the standard `HardwareSerial` class (`Serial`). No additional library installation is required.

---

## 2. Library

```cpp
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
```

`SerialBT` is the Bluetooth serial port object. It operates in parallel with `Serial` (USB), enabling simultaneous wired debug output and wireless command reception within the same sketch.

| USB Serial | Bluetooth Serial |
|---|---|
| `Serial.begin(115200)` | `SerialBT.begin("Shas_ESP32")` |
| `Serial.available()` | `SerialBT.available()` |
| `Serial.readStringUntil('\n')` | `SerialBT.readStringUntil('\n')` |
| `Serial.println(...)` | `SerialBT.println(...)` |

The string argument passed to `SerialBT.begin()` defines the Bluetooth device name broadcast during discovery. This is the identifier visible on the host device during a scan.

---

## 3. Hardware Configuration

| Signal | ESP32 GPIO |
|---|---|
| Relay IN (Bulb B3) | GPIO 25 |
| SDA (LCD) | GPIO 21 |
| SCL (LCD) | GPIO 22 |

The relay module is **active-LOW**. The initial state must be set to `HIGH` in `setup()` to ensure the relay remains de-energised at power-on, prior to any command being received.

```cpp
pinMode(B3, OUTPUT);
digitalWrite(B3, HIGH);   // Relay OFF — prevents spurious activation at startup
```

---

## 4. Phone Application — Serial Bluetooth Terminal

Install **Serial Bluetooth Terminal** (developer: Kai Morich) from the Google Play Store. This application functions as a wireless serial monitor, transmitting user-entered text to the ESP32 and displaying responses received over the Bluetooth link.

The application appends a newline character (`\n`) to each transmitted string upon pressing Send. The ESP32 sketch uses this as the message terminator in `readStringUntil('\n')`.

---

## 5. Pairing Procedure (One-Time Per Device Name)

Pairing is performed through the Android system Bluetooth settings, not within the Serial Bluetooth Terminal application itself.

1. Upload the sketch to the ESP32 and power it on
2. Open Serial Monitor at **115200 baud** — confirm the message:
   `Bluetooth connection started! Device name: Shas_ESP32`
3. On the Android device: **Settings → Bluetooth → Enable → Scan**
4. Select `Shas_ESP32` from the discovered devices list
5. Enter PIN **`1234`** when prompted — the device shows **Paired**
6. Open Serial Bluetooth Terminal → tap the plug icon → select `Shas_ESP32` → **Connected**

> ⚠️ If the device name changes between sketches (e.g., from `ESP32_TEST` to `Shas_ESP32`), the pairing process must be repeated for the new name.

---

## 6. Connection Test — `BT_Connection_test.ino`

This sketch validates the Bluetooth link before any hardware control is introduced. The ESP32 echoes every received string back to the phone, confirming bidirectional communication integrity.

**Expected behaviour:**

| Action | Phone display | Serial Monitor |
|---|---|---|
| Send `hello` | `You sent: hello` | `Received via BT: hello` |
| Send `B3ON` | `You sent: B3ON` | `Received via BT: B3ON` |
| Type in Serial Monitor | `PC says: <text>` | — |

`cmd.trim()` is called on every received string to strip carriage return (`\r`) and other invisible characters appended by certain platforms. Omitting this step causes string comparisons to fail silently.

---

## 7. Bulb B3 Control — `BT_Bulb3_test.ino`

This sketch extends the connection test with relay control and LCD status feedback on the 16×2 I2C display (`0x27`).

**Command set:**

| Command (send from phone) | Relay State | LCD Line 2 | Phone Reply |
|---|---|---|---|
| `B3ON` | GPIO 25 → LOW → B3 ON | `Status: ON ` | `Bulb-3 ON` |
| `B3OFF` | GPIO 25 → HIGH → B3 OFF | `Status: OFF` | `Bulb-3 OFF` |
| Any other string | No change | No change | `Unknown Command` |

**LCD update strategy:** Line 2 is overwritten in-place each loop iteration using `setCursor(0,1)`. `lcd.clear()` is deliberately avoided in `loop()` to prevent display flicker. A trailing space following `"ON "` ensures the third character position is overwritten when transitioning from `"OFF"` to `"ON"`, preventing stale character artefacts.

---

## 8. Staged Validation Sequence

| Step | Sketch | Validates |
|---|---|---|
| 1 | `BT_Connection_test.ino` | Pairing, bidirectional serial link |
| 2 | `BT_Bulb3_test.ino` | Command parsing, relay switching, LCD feedback |

Proceed to Step 2 only after Step 1 confirms clean echo behaviour. This isolates communication faults from hardware control faults.

---

## 9. Common Issues

| Symptom | Likely Cause | Resolution |
|---|---|---|
| Device not found during scan | Sketch not running or wrong baud rate | Confirm Serial Monitor shows startup message |
| Command received but relay does not respond | `trim()` missing — `\r` appended to command | Ensure `cmd.trim()` is called before comparison |
| LCD shows `ON F` after switching from OFF | Missing trailing space in `"ON "` | Use `"ON "` (with space), not `"ON"` |
| Relay activates briefly at power-on | `digitalWrite HIGH` missing in `setup()` | Always initialise relay pin HIGH before loop |
| `Unknown Command` for correct input | Case mismatch — `b3on` ≠ `B3ON` | Commands are case-sensitive; use exact strings |