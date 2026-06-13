#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = ".";
const char* password = "987654321";

// GPIO pins (change if needed)
#define light1 D1
#define light2 D2
#define fan    D5
#define plug   D6

ESP8266WebServer server(80);

IPAddress local_IP(192,168,10,1);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

// ---------------- DEVICE HANDLERS ----------------
void onDevice(int pin, const char* name) {
  Serial.printf("%s ON\n", name);
  digitalWrite(pin, HIGH);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void offDevice(int pin, const char* name) {
  Serial.printf("%s OFF\n", name);
  digitalWrite(pin, LOW);
  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

// Light 1
void l1on()  { onDevice(light1, "Light1"); }
void l1off() { offDevice(light1, "Light1"); }

// Light 2
void l2on()  { onDevice(light2, "Light2"); }
void l2off() { offDevice(light2, "Light2"); }

// Fan
void fon()   { onDevice(fan, "Fan"); }
void foff()  { offDevice(fan, "Fan"); }

// Plug
void pon()   { onDevice(plug, "Plug"); }
void poff()  { offDevice(plug, "Plug"); }

// ---------------- HTML PAGE ----------------
void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Smart Home</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
      font-family: Arial;
      text-align: center;
      background: #eef2f3;
    }

    h1 {
      margin-top: 20px;
    }

    .card {
      background: white;
      width: 320px;
      margin: 15px auto;
      padding: 15px;
      border-radius: 15px;
      box-shadow: 0 4px 10px rgba(0,0,0,0.2);
    }

    .title {
      font-size: 18px;
      margin-bottom: 10px;
      font-weight: bold;
    }

    .btn {
      display: inline-block;
      width: 120px;
      padding: 10px;
      margin: 5px;
      border-radius: 10px;
      text-decoration: none;
      color: white;
      font-weight: bold;
    }

    .on { background: #28a745; }
    .off { background: #dc3545; }
  </style>
</head>

<body>

<h1>-- Smart Home Control --</h1>

<!-- Light 1 -->
<div class="card">
  <div class="title"> Light 1</div>
  <a href="/l1on" class="btn on">ON</a>
  <a href="/l1off" class="btn off">OFF</a>
</div>

<!-- Light 2 -->
<div class="card">
  <div class="title"> Light 2</div>
  <a href="/l2on" class="btn on">ON</a>
  <a href="/l2off" class="btn off">OFF</a>
</div>


<!-- Fan -->
<div class="card">
  <div class="title"> Fan</div>
  <a href="/fon" class="btn on">ON</a>
  <a href="/foff" class="btn off">OFF</a>
</div>

<!-- Plug -->
<div class="card">
  <div class="title"> Plug Board</div>
  <a href="/pon" class="btn on">ON</a>
  <a href="/poff" class="btn off">OFF</a>
</div>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);

  pinMode(light1, OUTPUT);
  pinMode(light2, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(plug, OUTPUT);

  digitalWrite(light1, LOW);
  digitalWrite(light2, LOW);
  digitalWrite(fan, LOW);
  digitalWrite(plug, LOW);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected");
  Serial.println(WiFi.localIP());

  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("Home-automation", "123456789");

  Serial.println(WiFi.softAPIP());

  // Routes
  server.on("/", handleRoot);

  server.on("/l1on", l1on);
  server.on("/l1off", l1off);

  server.on("/l2on", l2on);
  server.on("/l2off", l2off);

  server.on("/fon", fon);
  server.on("/foff", foff);

  server.on("/pon", pon);
  server.on("/poff", poff);

  server.begin();
  Serial.println("HTTP Server Started");
}

// ---------------- LOOP ----------------
void loop() {
  server.handleClient();
}
