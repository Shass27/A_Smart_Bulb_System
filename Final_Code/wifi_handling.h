#ifndef WIFI_HANDLING_H
#define WIFI_HANDLING_H

void handleWiFi() {
  WiFiClient client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();
  Serial.println("WEB REQ: " + request);

  // Bulb control
  if (request.indexOf("GET /B1ON") >= 0) {
    bulb1On = true;
    timerB1 = millis();
    digitalWrite(PIN_B1, LOW);
  }
  if (request.indexOf("GET /B1OFF") >= 0) {
    bulb1On = false;
    digitalWrite(PIN_B1, HIGH);
  }

  if (request.indexOf("GET /B2ON") >= 0) {
    bulb2On = true;
    timerB2 = millis();
    digitalWrite(PIN_B2, LOW);
  }
  if (request.indexOf("GET /B2OFF") >= 0) {
    bulb2On = false;
    digitalWrite(PIN_B2, HIGH);
  }

  if (request.indexOf("GET /B3ON") >= 0) {
    bulb3On = true;
    digitalWrite(PIN_B3, LOW);
  }
  if (request.indexOf("GET /B3OFF") >= 0) {
    bulb3On = false;
    digitalWrite(PIN_B3, HIGH);
  }

  if (request.indexOf("GET /ALLON") >= 0) {
    bulb1On = bulb2On = bulb3On = true;
    timerB1 = timerB2 = millis();
    digitalWrite(PIN_B1, LOW);
    digitalWrite(PIN_B2, LOW);
    digitalWrite(PIN_B3, LOW);
  }
  if (request.indexOf("GET /ALLOFF") >= 0) {
    bulb1On = bulb2On = bulb3On = false;
    digitalWrite(PIN_B1, HIGH);
    digitalWrite(PIN_B2, HIGH);
    digitalWrite(PIN_B3, HIGH);
  }

  // ---------- HTML GUI ----------
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type:text/html");
  client.println();

  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>";
  html += "<title>Smart Lighting</title>";
  html += "<link href='https://fonts.googleapis.com/css2?family=Rajdhani:wght@400;600;700&display=swap' rel='stylesheet'>";
  html += "<style>";
  html += "*{margin:0;padding:0;box-sizing:border-box}";
  html += "body{background:#090d18;color:#e0e6f0;font-family:'Rajdhani',sans-serif;min-height:100vh;display:flex;flex-direction:column;align-items:center;padding:32px 16px}";
  html += "h1{font-size:1.5rem;font-weight:700;letter-spacing:5px;text-transform:uppercase;color:#60a5fa;margin-bottom:4px}";
  html += ".sub{font-size:0.72rem;letter-spacing:3px;color:#334155;margin-bottom:30px;text-transform:uppercase}";
  html += ".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(215px,1fr));gap:14px;width:100%;max-width:760px}";
  html += ".card{background:#0f172a;border:1px solid #1e3a5f;border-radius:14px;padding:22px 18px;display:flex;flex-direction:column;gap:16px;transition:border-color .3s}";
  html += ".card:hover{border-color:#3b82f6}";
  html += ".card-top{display:flex;align-items:center;gap:12px}";
  html += ".dot{width:11px;height:11px;border-radius:50%;flex-shrink:0;transition:box-shadow .4s}";
  html += ".on{background:#22c55e;box-shadow:0 0 10px #22c55e,0 0 20px #16a34a55}";
  html += ".off{background:#1e293b;box-shadow:none}";
  html += ".info .name{font-size:1.05rem;font-weight:700;letter-spacing:2px;color:#cbd5e1}";
  html += ".info .mode{font-size:0.68rem;letter-spacing:1.5px;color:#334155;text-transform:uppercase;margin-top:2px}";
  html += ".btns{display:flex;gap:8px}";
  html += "a{text-decoration:none;flex:1}";
  html += ".btn{width:100%;padding:10px 0;border:none;border-radius:8px;font-family:'Rajdhani',sans-serif;font-size:0.88rem;font-weight:700;letter-spacing:2px;cursor:pointer;transition:all .2s}";
  html += ".bon{background:linear-gradient(135deg,#1d4ed8,#2563eb);color:#fff}";
  html += ".bon:hover{background:linear-gradient(135deg,#2563eb,#3b82f6);box-shadow:0 0 14px #3b82f655}";
  html += ".boff{background:#1e293b;color:#475569;border:1px solid #1e3a5f}";
  html += ".boff:hover{background:#263347;color:#94a3b8}";
  html += ".act{box-shadow:0 0 12px rgba(59,130,246,.45)}";
  html += ".master{display:flex;gap:12px;width:100%;max-width:760px;margin-top:6px}";
  html += ".master a{flex:1}";
  html += ".master .btn{padding:13px 0;font-size:0.95rem;letter-spacing:3px}";
  html += ".allon{background:linear-gradient(135deg,#064e3b,#059669);color:#d1fae5}";
  html += ".allon:hover{box-shadow:0 0 16px #05966955}";
  html += ".alloff{background:#0f172a;color:#475569;border:1px solid #1e3a5f}";
  html += ".footer{margin-top:28px;font-size:0.65rem;letter-spacing:3px;color:#1e293b;text-transform:uppercase}";
  html += "</style></head><body>";
  html += "<h1>&#9729; Smart Lighting</h1><p class='sub'>ESP32 Control Panel</p>";
  html += "<div class='grid'>";

  html += "<div class='card'><div class='card-top'><div class='dot " + String(bulb1On ? "on" : "off") + "'></div>";
  html += "<div class='info'><div class='name'>BULB B1</div><div class='mode'>IR + Ultrasonic</div></div></div>";
  html += "<div class='btns'><a href='/B1ON'><button class='btn bon" + String(bulb1On ? " act" : "") + "'>ON</button></a>";
  html += "<a href='/B1OFF'><button class='btn boff" + String(!bulb1On ? " act" : "") + "'>OFF</button></a></div></div>";

  html += "<div class='card'><div class='card-top'><div class='dot " + String(bulb2On ? "on" : "off") + "'></div>";
  html += "<div class='info'><div class='name'>BULB B2</div><div class='mode'>PIR Motion</div></div></div>";
  html += "<div class='btns'><a href='/B2ON'><button class='btn bon" + String(bulb2On ? " act" : "") + "'>ON</button></a>";
  html += "<a href='/B2OFF'><button class='btn boff" + String(!bulb2On ? " act" : "") + "'>OFF</button></a></div></div>";

  html += "<div class='card'><div class='card-top'><div class='dot " + String(bulb3On ? "on" : "off") + "'></div>";
  html += "<div class='info'><div class='name'>BULB B3</div><div class='mode'>Bluetooth</div></div></div>";
  html += "<div class='btns'><a href='/B3ON'><button class='btn bon" + String(bulb3On ? " act" : "") + "'>ON</button></a>";
  html += "<a href='/B3OFF'><button class='btn boff" + String(!bulb3On ? " act" : "") + "'>OFF</button></a></div></div>";

  html += "</div>";
  html += "<div class='master'><a href='/ALLON'><button class='btn allon'>&#9889; ALL ON</button></a>";
  html += "<a href='/ALLOFF'><button class='btn alloff'>ALL OFF</button></a></div>";
  html += "<p class='footer'>ESP32 Smart Lighting v1.0</p>";
  html += "</body></html>";
  client.println(html);
  client.stop();
}

#endif
