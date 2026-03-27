void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 initialised");
}

void loop() {
  Serial.println("Hello from ESP32!");
  delay(100);
}