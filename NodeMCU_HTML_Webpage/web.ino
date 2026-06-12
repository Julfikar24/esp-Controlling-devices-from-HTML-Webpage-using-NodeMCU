#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = ".";
const char* password = "987654321";

#define led D5

ESP8266WebServer server(80);

IPAddress local_IP(192,168,10,1);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

void handleLedOn() {
  digitalWrite(led, HIGH);
  server.send(200, "text/plain", "LED ON");
}

void handleLedOff() {
  digitalWrite(led, LOW);
  server.send(200, "text/plain", "LED OFF");
}

void setup() {
  Serial.begin(9600);

  pinMode(led, OUTPUT);

  WiFi.mode(WIFI_AP_STA);

  WiFi.begin(ssid, password);

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connected");

  Serial.print("Router IP: ");
  Serial.println(WiFi.localIP());

  WiFi.softAPConfig(local_IP, gateway, subnet);

  WiFi.softAP(
    "Home-automation",
    "123456789"
  );

  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/ledon", handleLedOn);
  server.on("/ledoff", handleLedOff);

  server.begin();

  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();
}
