#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("Shas_ESP32");
  Serial.println("Bluetooth connection started! Device name: Shas_ESP32");
}

void loop() {
  if (SerialBT.available()) {
    String incoming = SerialBT.readStringUntil('\n'); // reding until newline
    incoming.trim(); // remove whitespace

    Serial.print("Recived via BT: ");
    Serial.println(incoming);

    //echo back to phone
    SerialBT.println("You sent: " + incoming);
  }

  // debugging
  if (Serial.available()) {
    String outgoing = Serial.readStringUntil('\n');
    SerialBT.println("PC says: " + outgoing);
  }
}
