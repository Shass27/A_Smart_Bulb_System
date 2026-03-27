#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // (addr, col, row)

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Shass27 test");
  lcd.setCursor(0,1);
  lcd.print("Successfull");
}

void loop() {

}