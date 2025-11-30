#include "occupancy.hpp"

OccupancyCounter::OccupancyCounter(int trigPin1, int echoPin1, int trigPin2,
                                   int echoPin2)
    : _trigPin1(trigPin1),
      _echoPin1(echoPin1),
      _trigPin2(trigPin2),
      _echoPin2(echoPin2),
      _total1(0),
      _total2(0),
      _occupancy(0),
      _lastDist1(0),
      _lastDist2(0),
      _lastAvg1(0),
      _lastAvg2(0),
      _lastTriggered1(false),
      _lastTriggered2(false) {}

void OccupancyCounter::begin() {
  pinMode(_trigPin1, OUTPUT);
  pinMode(_echoPin1, INPUT);
  pinMode(_trigPin2, OUTPUT);
  pinMode(_echoPin2, INPUT);
}

long OccupancyCounter::readDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2.0;
}

void OccupancyCounter::update() {
  int dist1 = readDistance(_trigPin1, _echoPin1);
  int dist2 = readDistance(_trigPin2, _echoPin2);

  _lastDist1 = dist1;
  _lastDist2 = dist2;

  if (dist1 >= SENSOR_MIN_DIST && dist1 <= SENSOR_MAX_DIST) {
    if (_s1Dists.size() >= WINDOW_SIZE) {
      int old = _s1Dists.front();
      _s1Dists.pop_front();
      _total1 -= old;
    }
    _s1Dists.push_back(dist1);
    _total1 += dist1;
  }

  if (dist2 >= SENSOR_MIN_DIST && dist2 <= SENSOR_MAX_DIST) {
    if (_s2Dists.size() >= WINDOW_SIZE) {
      int old = _s2Dists.front();
      _s2Dists.pop_front();
      _total2 -= old;
    }
    _s2Dists.push_back(dist2);
    _total2 += dist2;
  }

  int avg1 = _s1Dists.size() > 0 ? _total1 / (int)_s1Dists.size() : 0;
  int avg2 = _s2Dists.size() > 0 ? _total2 / (int)_s2Dists.size() : 0;

  _lastAvg1 = avg1;
  _lastAvg2 = avg2;

  // Trigger if current reading differs significantly from average
  bool triggered1 = _s1Dists.size() >= WINDOW_SIZE - 5 &&
                    dist1 >= SENSOR_MIN_DIST && dist1 <= SENSOR_MAX_DIST &&
                    abs(dist1 - avg1) > 4;
  bool triggered2 = _s2Dists.size() >= WINDOW_SIZE - 5 &&
                    dist2 >= SENSOR_MIN_DIST && dist2 <= SENSOR_MAX_DIST &&
                    abs(dist2 - avg2) > 4;

  _lastTriggered1 = triggered1;
  _lastTriggered2 = triggered2;

  if (triggered1 && !triggered2)
    _occupancy++;
  else if (!triggered1 && triggered2)
    _occupancy--;
}

int OccupancyCounter::getOccupancy() const { return _occupancy; }

int OccupancyCounter::getDistance1() const { return _lastDist1; }

int OccupancyCounter::getDistance2() const { return _lastDist2; }

int OccupancyCounter::getAverage1() const { return _lastAvg1; }

int OccupancyCounter::getAverage2() const { return _lastAvg2; }

bool OccupancyCounter::isTriggered1() const { return _lastTriggered1; }

bool OccupancyCounter::isTriggered2() const { return _lastTriggered2; }
