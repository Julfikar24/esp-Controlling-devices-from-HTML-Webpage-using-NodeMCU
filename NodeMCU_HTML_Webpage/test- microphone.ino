#include <I2S.h>

void setup() {
  Serial.begin(115200);

  if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("I2S init failed!");
    while (1);
  }

  Serial.println("INMP441 Test Started");
}

void loop() {

  if (I2S.available()) {

    int32_t sample = I2S.read();

    Serial.println(sample);
  }
}
