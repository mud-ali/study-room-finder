#pragma once

#include <string>
#include <vector>

typedef struct roomData {
  std::string name;
  unsigned int occupancy;
  unsigned int volume;
  unsigned int maxOccupancy;
} roomData;

extern std::vector<roomData> allRooms;