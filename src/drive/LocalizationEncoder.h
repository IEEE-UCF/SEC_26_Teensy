/**
 * @file LocalizationEncoder.h
 * @brief Tracks the robot's position using encoder counts and a gyro yaw reading.
 *
 * Maintains the robot's position by processing encoder data from three wheels
 * along with yaw readings from a gyroscope.
 *
 * @author Aldem Pido
 */

#ifndef LOCALIZATIONENCODER_H
#define LOCALIZATIONENCODER_H

#include <Arduino.h>

#include "MOTORCONFIG.h"
#include "math/Pose2D.h"

#define STARTX 30.5  ///< Initial X position
#define STARTY 6     ///< Initial Y position

using namespace MotorConstants;

class LocalizationEncoder {
 public:
  LocalizationEncoder();
  void updatePosition(const long encoderCounts[3], float yaw);
  Pose2D getPosition() const;
  void setPosition(const Pose2D &transform);
  void PrintInfo(Print &output) const;
  friend Print &operator<<(Print &output, const LocalizationEncoder &transform);

 private:
  Pose2D transform;
  long previousLeftTicks = 0;
  long previousBackTicks = 0;
  long previousRightTicks = 0;
  float previousYaw = 0;
};

#endif
