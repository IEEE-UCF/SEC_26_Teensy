#ifndef SIMPLEROBOTDRIVE_H
#define SIMPLEROBOTDRIVE_H

#include "DriveMotor.h"
#include "LocalizationEncoder.h"
#include <Arduino.h>
#include <Print.h>

class SimpleRobotDrive
{
public:
  SimpleRobotDrive(const MotorSetup motorSetups[], int numMotors);
  void Begin();
  void Set(int motorDirectSpeed[]);
  void SetIndex(int motorDirectSpeed, int index);
  void ReadAll(); // Renamed function
  void Write();
  void PrintInfo(Print &output, bool printConfig = false) const;
  void PrintLocal(Print &output) const;
  Pose2D GetPosition() const;

protected:
  int numMotors;
  long *enc;
  DriveMotor **motors;
  LocalizationEncoder localization;
  void ReadEnc();
  long *GetEnc();
};

Print &operator<<(Print &output, const SimpleRobotDrive &drive);

#endif // SIMPLEROBOTDRIVE_H
