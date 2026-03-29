# ESP32 Smart Lighting System
## Wi-Fi Connectivity Test and Web Server Control

---

## Overview

The following steps will introduce Wi-Fi functionality to the ESP32 Smart Lighting System.  
Step 1 validates that the ESP32 can connect to a local Wi-Fi network and obtain an IP address.  
Step 2 extends this by hosting an HTTP web server, allowing Bulb B3 to be controlled remotely from any browser on the same network.

---

## Step 1 — `ESP32_wifi_connection`

### Objective
Verify that the ESP32 successfully connects to the configured Wi-Fi network and displays the assigned IP address on both the Serial Monitor and the I2C LCD.

### Hardware Required
| Component | Quantity |
|---|---|
| ESP32 Dev Board | 1 |
| 16×2 LCD (I2C, PCF8574 backpack) | 1 |

### Pin Connections (LCD)
| LCD (I2C) Pin | ESP32 Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### Libraries Required
- `WiFi.h` — ESP32 built-in
- `Wire.h` — ESP32 built-in
- `LiquidCrystal_I2C` (by Frank de Brabander) — install via Library Manager

### Configuration
Before uploading, set your network credentials in the sketch:
```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```

### Sketch Behaviour
1. LCD displays `"Wi-Fi Test"` and `"Connecting..."` during the connection attempt.
2. The ESP32 attempts to connect for up to 15 seconds (30 × 500 ms polling intervals).
3. On success — Serial Monitor prints the assigned IP address; LCD displays `"Connected!"` and the IP.
4. On failure — Serial Monitor and LCD both display a connection failure message.

### Expected Serial Monitor Output
```
Connecting to Wi-Fi: YOUR_SSID
..........
Wi-Fi connected!
IP address: 192.168.x.x
```

> **Note:** In the current sketch, the second `while` loop (checking `WL_CONNECTED`) should be replaced with `if` to allow execution to proceed to the LCD update block. As written, the LCD will not update on a successful connection. This is flagged for correction before Step 2.

---

## Step 2 — `ESP32_wifi_webserver`

### Objective
Host a lightweight HTTP web server on the ESP32 that serves an HTML control page, allowing Bulb B3 to be switched ON or OFF from any browser on the same Wi-Fi network.

### Hardware Required
| Component | Quantity |
|---|---|
| ESP32 Dev Board | 1 |
| 16×2 LCD (I2C, PCF8574 backpack) | 1 |
| 1-Channel Relay Module | 1 |
| AC Bulb (B3) or LED (for bench testing) | 1 |

### Pin Connections
| Component | ESP32 Pin |
|---|---|
| Relay IN (B3) | GPIO 25 |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |

### Libraries Required
Same as Step 1 (`WiFi.h`, `Wire.h`, `LiquidCrystal_I2C`).

### Configuration
```cpp
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
```
> ⚠ Remove actual credentials before committing to version control.

### Sketch Behaviour
1. On startup, the ESP32 connects to Wi-Fi and starts an HTTP server on **port 80**.
2. The assigned IP address is displayed on the LCD (row 2).
3. The `loop()` continuously listens for incoming browser clients.
4. When a client request is received, the sketch parses the URL path:
   - `/B3ON`  → sets GPIO 25 LOW (relay activates, bulb ON)
   - `/B3OFF` → sets GPIO 25 HIGH (relay deactivates, bulb OFF)
5. An HTML response page with ON/OFF buttons is served back to the browser.
6. Current bulb state is reflected on the LCD (row 2) and in the web page status line.

### Relay Logic (Active-LOW)
| GPIO 25 State | Relay State | Bulb B3 |
|---|---|---|
| HIGH (default) | De-energised | OFF |
| LOW | Energised | ON |

### How to Test
1. Upload the sketch and open Serial Monitor at **115200 baud**.
2. Note the IP address printed (e.g., `192.168.1.105`).
3. On any device connected to the same Wi-Fi network, open a browser and navigate to:
   ```
   http://192.168.1.105
   ```
4. The control page will load with **ON** and **OFF** buttons for Bulb B3.
5. Clicking ON sends a `GET /B3ON` request; clicking OFF sends `GET /B3OFF`.
6. Verify relay click and bulb state change; confirm LCD updates accordingly.

### Web Interface
The page is served as a minimal HTML document with inline CSS. It presents two hyperlink-wrapped buttons labelled **ON** (green) and **OFF** (red), and a status line showing the current state of B3. No external libraries or JavaScript are used — the page is fully static and browser-agnostic.

---

## Summary

| Step | Sketch | Function |
|---|---|---|
| 1 | `ESP32_wifi_connection` | Validates Wi-Fi connectivity and IP assignment |
| 2 | `ESP32_wifi_webserver` | Hosts HTTP server; controls B3 via browser |

These two steps together form the foundation for the full multi-bulb Wi-Fi control implemented in Step 11 (`ESP32_wifi_lcd_GUI`).
