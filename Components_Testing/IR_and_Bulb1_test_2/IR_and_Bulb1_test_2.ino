#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define IR 15
#define B1 4

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long irTimer = 0;
unsigned long lastUpdate = 0;
const unsigned long ON_TIME = 60000; // 1 min
bool relayState = false;

int prevIR = HIGH;

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
    relayState = true;
    digitalWrite(B1, LOW);
    irTimer = millis();
    lastUpdate = 0;

    Serial.println("IR DETECTED (<3cm)");
  }
  
  if (relayState) {
    unsigned long elapsed = millis() - irTimer;
    unsigned long remaining = (ON_TIME - elapsed)/1000; //seconds

    if (millis() - lastUpdate >= 1000) { //updates LCD panel every 1 second compared to the earlier test
      lastUpdate = millis();

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("B1: ON");
      lcd.setCursor(0, 1);
      lcd.print("TIME LEFT:");
      lcd.print(remaining);
      lcd.print("s");

      Serial.print("B1 ON - Time Left: ");
      Serial.print(remaining);
      Serial.println(" sec");
    }

    if (elapsed >= ON_TIME) {
      relayState = false;
      digitalWrite(B1, HIGH);

      Serial.println("TIMEOUT -> B1 OFF");

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("IR: NOT DETECTED");
      lcd.setCursor(0, 1);
      lcd.print("B1: OFF");
    }
  }

  if (!relayState && irValue == HIGH) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IR: NO DETECTION ");
    lcd.setCursor(0, 1);
    lcd.print("B1: OFF");
  }
}