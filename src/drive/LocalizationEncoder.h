/*
Aldem Pido - 4/1/25
LocalizationEncoder.h - tracks robot's position using encoder counts and a gyro
yaw reading
*/
#ifndef LOCALIZATIONENCODER_H
#define LOCALIZATIONENCODER_H

#include <Arduino.h>

#include "MOTORCONFIG.h"
#include "math/Pose2D.h"

#define STARTX 30.5
#define STARTY 6
using namespace MotorConstants;

class LocalizationEncoder {
 public:
  LocalizationEncoder();
  void updatePosition(const long encoderCounts[3], float yaw);
  Pose2D getPosition() const;  // Added const
  void setPosition(const Pose2D &transform);
  void PrintInfo(Print &output) const;  // Added const
  friend Print &operator<<(Print &output, const LocalizationEncoder &transform);

 private:
  Pose2D transform;
  long previousLeftTicks = 0;
  long previousBackTicks = 0;
  long previousRightTicks = 0;
  float previousYaw = 0;
};
#endif
