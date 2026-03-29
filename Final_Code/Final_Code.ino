// IMPORTANT: In Arduino IDE, set Tools -> Partition Scheme to "Huge APP (3MB No OTA/1MB SPIFFS)".
// The default 1.2MB APP partition causes "text section exceeds available space in board" compile error.
// Use this partition setting before building/uploading this sketch on ESP32.

#include <WiFi.h>
#include <BluetoothSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR 15
#define PIR 27

#define PIN_B1 4
#define PIN_B2 26
#define PIN_B3 25

const char* ssid     = "Jio";
const char* password = "5@ty3ndr@BOSE";

WiFiServer server(80);
BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool bulb1On = false;
bool bulb2On = false;
bool bulb3On = false;

const unsigned long ON_TIME = 60000;   // 1 minute
unsigned long timerB1 = 0;
unsigned long timerB2 = 0;

unsigned long lcdTimer = 0;

#include "functions.h"
#include "wifi_handling.h"

void setup() {
  Serial.begin(115200);

  pinMode(IR, INPUT);
  pinMode(PIR, INPUT);

  pinMode(PIN_B1, OUTPUT);
  pinMode(PIN_B2, OUTPUT);
  pinMode(PIN_B3, OUTPUT);

  digitalWrite(PIN_B1, HIGH);
  digitalWrite(PIN_B2, HIGH);
  digitalWrite(PIN_B3, HIGH);

  //lcd
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Smart Bulb System");

  ///BT
  SerialBT.begin("ESP32_SMART_HOME");
  Serial.println("Bluetooth Ready");

  //WiFi
  WiFi.mode(WIFI_STA); //enables to connect wifi
  WiFi.setSleep(false); //keeps the esp32 awake
  WiFi.begin(ssid, password);

  lcd.setCursor(0,1);
  lcd.print("Connecting WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  server.begin();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(2000);
  lcd.clear();
}

void loop() {
  handleIR();
  handlePIR();
  handleBluetooth();
  handleWiFi();
  handleTimers();
  updateLCD();
}
