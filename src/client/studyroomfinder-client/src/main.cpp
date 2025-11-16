#include <Arduino.h>

const int micPin = A0;
const int sampleMillis = 50;

void setup() {
  Serial.begin(115200);
  pinMode(micPin, INPUT);
}

void loop() {
  unsigned long startTime = millis();
  unsigned int volume = 0;
  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int currSample;

  while (millis() - startTime < sampleMillis) {
    currSample = analogRead(A0);
    if (currSample < 1023) {
      if (currSample > signalMax) {
        signalMax = currSample;
      }
      if (currSample < signalMin) {
        signalMin = currSample;
      }
    }
  }
  
  volume = signalMax - signalMin;

  Serial.print("Sound Amplitude: ");
  for (unsigned int i=0;i<volume;i+=2)
    Serial.print(".");
  Serial.print(volume);
  Serial.println();
}