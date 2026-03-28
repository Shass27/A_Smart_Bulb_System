#define PIR_PIN 27

unsigned long elapsed_ms = 0;
unsigned long high_start = 0;
bool wasHigh = false;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT_PULLUP);
}

void loop() {
  int state = digitalRead(PIR_PIN);
  elapsed_ms += 500;

  // Detect rising edge (LOW → HIGH)
  if (state == HIGH && !wasHigh) {
    high_start = elapsed_ms;
    Serial.println("PIR went HIGH");
    wasHigh = true;
  }

  // Detect falling edge (HIGH → LOW)
  if (state == LOW && wasHigh) {
    Serial.print("PIR HIGH duration: ");
    Serial.print(elapsed_ms - high_start);
    Serial.println(" ms");
    wasHigh = false;
  }

  Serial.print("State: "); Serial.print(state);
  Serial.print(" | Time: "); Serial.println(elapsed_ms);

  delay(500);
}