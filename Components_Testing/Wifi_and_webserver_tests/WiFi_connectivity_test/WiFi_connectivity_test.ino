#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = ""; //put in ur WiFiSSID
const char* password = ""; // put in ur wifi password

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Wi-Fi Test");

  lcd.setCursor(0,1);
  lcd.print("Connecting...");
  Serial.print("Connecting to Wi-Fi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) { // 15s buffer to establish connection
    delay(500);
    Serial.print(".");
    attempts++;
  }

  while(WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Wi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  lcd.clear();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Wi-Fi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    lcd.setCursor(0,0);
    lcd.print("Connected!");
    lcd.setCursor(0,1);
    lcd.print(WiFi.localIP());
  } else {
    Serial.println();
    Serial.println("Wi-Fi connection failed!");
    lcd.setCursor(0,0);
    lcd.print("Connection Fail");
    lcd.setCursor(0,1);
    lcd.print("Check credentials");
  }
}

void loop() {
}