/**
 * @file SimpleRobotDrive.h
 * @defgroup drives Robot Drive Systems
 * @ingroup drives
 * @brief Classes related to robot drive mechanisms.
 *
 * This group contains all drive-related classes used for robot motion.
 *
 * @author Aldem Pido
 */

#ifndef SIMPLEROBOTDRIVE_H
#define SIMPLEROBOTDRIVE_H

#include <Arduino.h>
#include <Print.h>

#include <memory>
#include <vector>

#include "DriveMotor.h"
#include "LocalizationEncoder.h"

/**
 * @class SimpleRobotDrive
 * @ingroup drives
 * @brief Base class for a robot drive system.
 */
class SimpleRobotDrive {
 public:
  SimpleRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output);
  void Begin();
  void Set(const int motorDirectSpeed[]);
  void SetIndex(int motorDirectSpeed, int index);
  void ReadAll(float yaw);
  void Write();
  virtual void PrintInfo(Print &output, bool printConfig = false) const;
  virtual void PrintLocal(Print &output) const;
  void SetPosition(const Pose2D &setPosition) { localization.setPosition(setPosition); }
  Pose2D GetPosition() const { return localization.getPosition(); }

 protected:
  const int numMotors;
  Print &output;
  elapsedMicros accelCall;
  std::unique_ptr<long[]> enc;
  std::vector<std::unique_ptr<DriveMotor>> motors;
  LocalizationEncoder localization;

  void ReadEnc();
  const long *GetEnc() const;

  friend Print &operator<<(Print &output, const SimpleRobotDrive &drive);
};

Print &operator<<(Print &output, const SimpleRobotDrive &drive);

#endif
