#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIR 27
#define B2 26  //connected through the relay... so relay<smth> terms can be related to the bulb

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ON_TIME = 60;  //seconds

unsigned long lastSecondTick = 0;  // "When did I last count down one second?"
unsigned long lastBlinkTick = 0;   // "When did I last blink the LCD?"

bool relayActive = false;  // "Is the bulb currently ON?"
bool blinkState = false;   // "Is the blink currently showing or hidden?"
int remainingTime = 0;     // "How many seconds left before bulb turns OFF?"

void setup() {
  Serial.begin(115200);

  pinMode(PIR, INPUT_PULLUP);
  pinMode(B2, OUTPUT);
  digitalWrite(B2, HIGH);  // Relay OFF (active LOW)

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("PIR SYSTEM");
  lcd.setCursor(0, 1);
  lcd.print("Warming up...");
  delay(30000);  // PIR warm-up

  lcd.clear();
  Serial.println("System ready");
}

void loop() {
  unsigned long int now = millis();
  bool pirState = digitalRead(PIR);

  //handling motion detection
  if (pirState == HIGH) {
    if (!relayActive) {
      Serial.println("Motion detected -> B2 ON");
      digitalWrite(B2, LOW);
    } else {
      Serial.println("Motion still detected -> Timer refreshed");
    }
    relayActive = true;
    remainingTime = ON_TIME;
    lastSecondTick = now;
  }

  //handling bulb timer
  if (relayActive && now - lastSecondTick >= 1000) {
    lastSecondTick = now;
    remainingTime--;

    Serial.print("Remaining Time: ");
    Serial.print(remainingTime);
    Serial.println(" sec");

    if (remainingTime <= 0) {
      relayActive = false;
      digitalWrite(B2, HIGH);  // Relay OFF
      Serial.println("Relay OFF (no motion)");
    }
  }

  //handling lcd blinking
  if (now - lastBlinkTick >= 500) {
    lastBlinkTick = now;
    blinkState = !blinkState;
  }

  //lcd
  lcd.setCursor(0, 0);
  lcd.print("PIR: ");
  lcd.print(pirState ? "DETECT " : "IDLE   ");

  lcd.setCursor(0, 1);
  lcd.print("Relay: ");
  lcd.print(relayActive ? (blinkState ? "ON  " : "    ") : "OFF ");
  lcd.print(" ");
  lcd.print(remainingTime);
  lcd.print("s  ");

  //debugging
  Serial.print("PIR State: ");
  Serial.println(pirState);

  delay(200);
}
