#define MIC_PIN 34

void setup() {
  Serial.begin(115200);
}

void loop() {
  int value = analogRead(MIC_PIN);
  Serial.println(value);
  delay(10);
}
