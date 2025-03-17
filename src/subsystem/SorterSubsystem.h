#ifndef SORTERSUBSYSTEM_H
#define SORTERSUBSYSTEM_H

#include <Arduino.h>
#include "../handler/TOFHandler.h"
#include "../handler/HallHandler.h"
#include "../handler/ServoHandler.h"
#include "../drive/DriveMotor.h"

#define OBJECT_RANGE 50
#define BOUNDS_MAG 2
#define SORTER_HALL_COUNT 3

class SortingSubsystem
{
public:
  SortingSubsystem(int iTOF, int *iHalls, int hallCount, int iServo, TOFHandler &tofs, HallHandler &halls, ServoHandler &servos, DriveMotor &transferMotor);
  void Begin();
  void Update();

  enum ServoPositions : uint8_t
  {
    LEFT = 30,   // Example value for left position
    CENTER = 90, // Example value for center position
    RIGHT = 150, // Example value for right position
  };

  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const SortingSubsystem &subsystem);

private:
  int iTOF;
  int *iHalls;
  int hallCount;
  int iServo;
  TOFHandler &tofs;
  HallHandler &halls;
  ServoHandler &servos;
  DriveMotor &transferMotor;

  int _state;
  int *_baseReadings;
};

#endif