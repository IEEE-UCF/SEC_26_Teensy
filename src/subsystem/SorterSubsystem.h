/*
Aldem Pido - 4/1/25
SorterSubsystem.h - hall magnet detect sorter
*/
#ifndef SORTERSUBSYSTEM_H
#define SORTERSUBSYSTEM_H

#include <Arduino.h>
#include "../handler/TOFHandler.h"
#include "../handler/HallHandler.h"
#include "../handler/ServoHandler.h"
#include "../drive/DriveMotor.h"
#include "../handler/RGBHandler.h"

#define OBJECT_RANGE 75
#define BOUNDS_MAG 2
#define SORTER_HALL_COUNT 3

class SorterSubsystem
{
public:
  SorterSubsystem(int iTOF, int hallCount, int iServo, TOFHandler &tofs, HallHandler &halls, ServoHandler &servos, DriveMotor &transferMotor, RGBHandler &rgb);
  void Begin();
  void Update();
  void MoveCenter();
  void MoveLeft();
  void MoveRight();
  void MoveSoftLeft();
  void MoveSoftRight();
  void SetState(int _state) { this->_state = _state; }

  enum ServoPositions : uint8_t
  {
    LEFT = 40, // Example value for left position
    SOFTLEFT = 70,
    CENTER = 90, // Example value for center position
    SOFTRIGHT = 110,
    RIGHT = 140, // Example value for right position
  };

  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const SorterSubsystem &subsystem);

private:
  int iTOF;
  int hallCount;
  int iServo;
  TOFHandler &tofs;
  HallHandler &halls;
  ServoHandler &servos;
  DriveMotor &transferMotor;
  RGBHandler &rgb;

  int _state;
  int *_baseReadings;
  bool objectMagnet;
};

#endif