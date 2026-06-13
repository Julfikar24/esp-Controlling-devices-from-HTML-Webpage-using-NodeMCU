#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = ".";
const char* password = "987654321";

// GPIO pins
#define light1 D1
#define light2 D2
#define fan    D5
#define plug   D6

ESP8266WebServer server(80);

// ---------------- STATUS ----------------
bool light1State = false;
bool light2State = false;
bool fanState = false;
bool plugState = false;

// ---------------- NETWORK ----------------
IPAddress local_IP(192,168,10,1);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

// ---------------- DEVICE CONTROL ----------------
void onDevice(int pin, const char* name, bool &state) {
  Serial.printf("%s ON\n", name);
  digitalWrite(pin, HIGH);
  state = true;

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

void offDevice(int pin, const char* name, bool &state) {
  Serial.printf("%s OFF\n", name);
  digitalWrite(pin, LOW);
  state = false;

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "");
}

// ---------------- ROUTES ----------------
void l1on()  { onDevice(light1, "Light1", light1State); }
void l1off() { offDevice(light1, "Light1", light1State); }

void l2on()  { onDevice(light2, "Light2", light2State); }
void l2off() { offDevice(light2, "Light2", light2State); }

void fon()   { onDevice(fan, "Fan", fanState); }
void foff()  { offDevice(fan, "Fan", fanState); }

void pon()   { onDevice(plug, "Plug", plugState); }
void poff()  { offDevice(plug, "Plug", plugState); }

// ---------------- STATUS API ----------------
void handleStatus() {
  String json = "{";
  json += "\"light1\":" + String(light1State) + ",";
  json += "\"light2\":" + String(light2State) + ",";
  json += "\"fan\":" + String(fanState) + ",";
  json += "\"plug\":" + String(plugState);
  json += "}";

  server.send(200, "application/json", json);
}

// ---------------- HTML ----------------
void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Smart Home Dashboard</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">

  <style>
    body {
      margin: 0;
      font-family: Arial;
      background: linear-gradient(135deg, #1e3c72, #2a5298);
      color: white;
      text-align: center;
    }

    h1 { margin: 20px 0; }

    .card {
      background: white;
      color: black;
      width: 320px;
      margin: 15px auto;
      padding: 15px;
      border-radius: 20px;
      box-shadow: 0 10px 25px rgba(0,0,0,0.3);
    }

    .title { font-weight: bold; margin-bottom: 10px; }

    .btn {
      display: inline-block;
      width: 120px;
      padding: 10px;
      margin: 5px;
      border-radius: 30px;
      text-decoration: none;
      color: white;
      font-weight: bold;
    }

    .on { background: #00c853; }
    .off { background: #d50000; }

    .dot {
      height: 10px;
      width: 10px;
      border-radius: 50%;
      display: inline-block;
      margin-right: 5px;
    }
  </style>
</head>

<body>

<h1>Smart Home Dashboard</h1>

<div class="card">
  <div class="title">Light 1</div>
  <a href="/l1on" class="btn on">ON</a>
  <a href="/l1off" class="btn off">OFF</a>
</div>

<div class="card">
  <div class="title">Light 2</div>
  <a href="/l2on" class="btn on">ON</a>
  <a href="/l2off" class="btn off">OFF</a>
</div>

<div class="card">
  <div class="title">Fan</div>
  <a href="/fon" class="btn on">ON</a>
  <a href="/foff" class="btn off">OFF</a>
</div>

<div class="card">
  <div class="title">Plug</div>
  <a href="/pon" class="btn on">ON</a>
  <a href="/poff" class="btn off">OFF</a>
</div>

<div class="card">
  <div class="title">Live Status</div>
  <div id="statusBox">Loading...</div>
</div>

<script>
setInterval(async () => {
  const res = await fetch('/status');
  const d = await res.json();

  document.getElementById("statusBox").innerHTML =
    "<span class='dot' style='background:" + (d.light1 ? "green" : "red") + "'></span> Light1: " + (d.light1 ? "ON" : "OFF") + "<br>" +
    "<span class='dot' style='background:" + (d.light2 ? "green" : "red") + "'></span> Light2: " + (d.light2 ? "ON" : "OFF") + "<br>" +
    "<span class='dot' style='background:" + (d.fan ? "green" : "red") + "'></span> Fan: " + (d.fan ? "ON" : "OFF") + "<br>" +
    "<span class='dot' style='background:" + (d.plug ? "green" : "red") + "'></span> Plug: " + (d.plug ? "ON" : "OFF");
}, 1000);
</script>

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

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("Home-automation", "123456789");

  server.on("/", handleRoot);

  server.on("/l1on", l1on);
  server.on("/l1off", l1off);

  server.on("/l2on", l2on);
  server.on("/l2off", l2off);

  server.on("/fon", fon);
  server.on("/foff", foff);

  server.on("/pon", pon);
  server.on("/poff", poff);

  server.on("/status", handleStatus);

  server.begin();
}

// ---------------- LOOP ----------------
void loop() {
  server.handleClient();
}
