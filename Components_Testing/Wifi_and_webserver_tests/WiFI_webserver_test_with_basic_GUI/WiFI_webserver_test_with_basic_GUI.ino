#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define B3 25

const char* ssid = "Jio";                //put in ur WiFiSSID
const char* password = "5@ty3ndr@BOSE";  // put in ur wifi password

WiFiServer server(80);
LiquidCrystal_I2C lcd(0x27, 16, 2);

bool bulb3State = false;

void setup() {
  Serial.begin(115200);

  pinMode(B3, OUTPUT);
  digitalWrite(B3, HIGH); 

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Connecting Wi-Fi");

  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts<30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if(attempts == 30) {
    lcd.clear();
    while(true) {
      lcd.setCursor(0,0);
      lcd.print("WiFi Connection Failed");
      lcd.setCursor(0,1);
      lcd.print("Check credentials");
    }
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wi-Fi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    String request = client.readStringUntil('\r');
    client.flush(); //reads necessary infromation from browser's request

    if(request.indexOf("/B3ON") != -1) {
      bulb3State = true;
      digitalWrite(B3, LOW);
    }
    if (request.indexOf("/B3OFF") != -1) {
      bulb3State = false;
      digitalWrite(B3, HIGH);  // OFF
    }

    // GUI
    String html = "<!DOCTYPE html><html><head><title>ESP32 Bulb Control</title>";
    html += "<style>";
    html += "body { font-family: Arial; text-align: center; margin-top:50px; }";
    html += "button { width:100px; height:50px; font-size:16px; margin:10px; }";
    html += "</style></head><body>";
    html += "<h2>ESP32 Bulb Control</h2>";

    html += "<p>Bulb-3: <a href=\"/B3ON\"><button style='background-color:lightgreen'>ON</button></a>";
    html += "<a href=\"/B3OFF\"><button style='background-color:lightcoral'>OFF</button></a></p>";

    html += "<p>Status: Bulb-3: " + String(bulb3State ? "ON" : "OFF") + "</p>";

    html += "</body></html>";

    client.println("HTTP/1.1 200 OK"); //tells the browser request succeded
    client.println("Content-type:text/html"); // informs the broswers to treat the following as html
    client.println();
    client.println(html); //renders the html string
    client.stop();

    lcd.setCursor(0, 1);
    lcd.print("B3:");
    lcd.print(bulb3State ? "ON " : "OFF");
  }
}
