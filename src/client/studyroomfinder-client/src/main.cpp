#include <Arduino.h>

#include "occupancy.hpp"

const int trigPin1 = 22;
const int echoPin1 = 23;

const int trigPin2 = 19;
const int echoPin2 = 21;

OccupancyCounter counter(trigPin1, echoPin1, trigPin2, echoPin2);

void setup() {
  Serial.begin(115200);
  counter.begin();
}

void loop() {
  counter.update();

  int nextDelay = 1000;

  if (!counter.isTriggered1() && !counter.isTriggered2()) {
    nextDelay -= 800;
  }

  Serial.print("Occupancy: ");
  Serial.println(counter.getOccupancy());

  delay(nextDelay);
}
