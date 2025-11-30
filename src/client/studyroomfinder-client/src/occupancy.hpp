#ifndef OCCUPANCY_HPP
#define OCCUPANCY_HPP

#include <Arduino.h>

#include <deque>

using std::deque;

class OccupancyCounter {
 public:
  OccupancyCounter(int trigPin1, int echoPin1, int trigPin2, int echoPin2);

  void begin();
  void update();
  int getOccupancy() const;

  // getters for debugging
  int getDistance1() const;
  int getDistance2() const;
  int getAverage1() const;
  int getAverage2() const;
  bool isTriggered1() const;
  bool isTriggered2() const;

 private:
  int _trigPin1;
  int _echoPin1;
  int _trigPin2;
  int _echoPin2;

  static const int SENSOR_MIN_DIST = 1;
  static const int SENSOR_MAX_DIST = 400;
  static const int WINDOW_SIZE = 50;

  // state vars
  int _total1;
  int _total2;
  int _occupancy;
  deque<int> _s1Dists;
  deque<int> _s2Dists;

  // cache data from last sensor readings for debug
  int _lastDist1;
  int _lastDist2;
  int _lastAvg1;
  int _lastAvg2;
  bool _lastTriggered1;
  bool _lastTriggered2;

  long readDistance(int trigPin, int echoPin);
};

#endif  // OCCUPANCY_HPP
