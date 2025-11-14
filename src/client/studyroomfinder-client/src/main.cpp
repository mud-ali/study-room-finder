#include <Arduino.h>

const int microphonePin = A0;

void setup() {
  pinMode(microphonePin, INPUT);
  Serial.begin(115200);
}

void loop() {
  Serial.print("volume level: ");
  int val = analogRead(microphonePin);
  Serial.println(val);
  delay(100);
}
