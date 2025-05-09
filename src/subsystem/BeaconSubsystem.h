/*
Aldem Pido - 4/1/25
BeaconSubsystem.h - used to control the beacon on the robot
*/
#ifndef BEACONSUBSYSTEM_H
#define BEACONSUBSYSTEM_H

#include <Arduino.h>

#include "../handler/ServoHandler.h"

class BeaconSubsystem {
 public:
  BeaconSubsystem(int indexBeacon, ServoHandler &servos);
  void Begin();
  void MoveUp();
  void WriteAngle(int angle);
  void MoveDown(int offset = 0);

  enum Positions : uint8_t { UP = 0, DOWN1 = 20, DOWN2 = 40, DOWN = 53 };

 private:
  int indexBeacon;
  ServoHandler &servos;
};

#endif