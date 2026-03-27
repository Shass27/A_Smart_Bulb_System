#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR 15
#define B1 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long irTimer = 0;
const unsigned long ON_TIME = 60000; // 1 min
bool bulbState = false;

void setup() {
  pinMode(IR, INPUT);
  pinMode(B1, OUTPUT);

  Serial.begin(115200);

  Wire.begin();
  delay(1000);

  digitalWrite(B1, HIGH);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("IR SENSOR TEST");
  lcd.setCursor(0,1);
  lcd.print("INITIALISING");

  Serial.println("Test Started");
}

void loop() {
  int irValue = digitalRead(IR);

  //shows LOW if object (<3 cm)
  if (irValue == LOW) {
    bulbState = true;
    digitalWrite(B1, LOW);
    irTimer = millis();

    Serial.println("IR DETECTED (<3cm)");

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IR: DETECTED");
    lcd.setCursor(0, 1);
    lcd.print("< 3cm --> B1 ON");
  }
  else {
    // IR not detected
    if (!bulbState) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("IR: NO DETECTION");
      lcd.setCursor(0, 1);
      lcd.print("Bulb: OFF");

      Serial.println("IR NOT DETECTED");
    }
  }

  // turning OFF bulb after 1 min
  if (bulbState && (millis() - irTimer >= ON_TIME)) {
    bulbState = false;
    digitalWrite(B1, HIGH);

    Serial.println("TIMEOUT... B1 is OFF");

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("IR: NO DETECTION");
    lcd.setCursor(0,1);
    lcd.print("Bulb: OFF");
  }
}