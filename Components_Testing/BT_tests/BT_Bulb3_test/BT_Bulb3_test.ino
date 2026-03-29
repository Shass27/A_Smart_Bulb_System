#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "BluetoothSerial.h"

#define B3 25

BluetoothSerial SerialBT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool B3State = false;

void setup() {
  Serial.begin(115200);

  pinMode(B3, OUTPUT);
  digitalWrite(B3, HIGH);   

  SerialBT.begin("Shas_ESP32");
  Serial.println("Bluetooth ready. Device name: Shas_ESP32");

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Bluetooth Bulb-3");
  lcd.setCursor(0,1);
  lcd.print("Status: OFF");
}

void loop() {
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n');
    cmd.trim();

    Serial.println("BT CMD: " + cmd);

    if (cmd == "B3ON") {
      digitalWrite(B3, LOW);   // Relay ON
      B3State = true;
      SerialBT.println("Bulb-3 ON");
    }
    else if (cmd == "B3OFF") {
      digitalWrite(B3, HIGH);  // Relay OFF
      B3State = false;
      SerialBT.println("Bulb-3 OFF");
    }
    else {
      SerialBT.println("Unknown Command");
    }
  }
}
