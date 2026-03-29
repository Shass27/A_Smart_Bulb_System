#ifndef FUNCTIONS_H
#define FUNCTIONS_H

void handleIR() {
  if (digitalRead(IR) == LOW) {
    bulb1On = true;
    timerB1 = millis();
    digitalWrite(PIN_B1, LOW);
  }
}

void handlePIR() {
  if (digitalRead(PIR) == HIGH) {
    bulb2On = true;
    timerB2 = millis();
    digitalWrite(PIN_B2, LOW);
  }
}

void handleBluetooth() {
  if (SerialBT.available()) {
    String cmd = SerialBT.readStringUntil('\n');
    cmd.trim();

    if (cmd == "B3ON") {
      bulb3On = true;
      digitalWrite(PIN_B3, LOW);
      SerialBT.println("B3 ON");
    }
    else if (cmd == "B3OFF") {
      bulb3On = false;
      digitalWrite(PIN_B3, HIGH);
      SerialBT.println("B3 OFF");
    }
  }
}

void handleTimers() {
  if (bulb1On && millis() - timerB1 >= ON_TIME) {
    bulb1On = false;
    digitalWrite(PIN_B1, HIGH);
  }

  if (bulb2On && millis() - timerB2 >= ON_TIME) {
    bulb2On = false;
    digitalWrite(PIN_B2, HIGH);
  }
}

void updateLCD() {
  if (millis() - lcdTimer < 1000) return;
  lcdTimer = millis();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("IR  PIR  BT");

  lcd.setCursor(0,1);
  lcd.print(bulb1On ? "ON " : "OFF");
  lcd.print(" ");
  lcd.print(bulb2On ? "ON " : "OFF");
  lcd.print(" ");
  lcd.print(bulb3On ? "ON " : "OFF");
}

#endif
