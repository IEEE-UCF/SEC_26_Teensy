/*
Aldem Pido - 4/1/25
PIDDriveController.h - fuse different PIDs needed for the robot control
*/

#ifndef PIDDRIVECONTROLLER_H
#define PIDDRIVECONTROLLER_H

#include "Arduino.h"
#include "MOTORCONFIG.h"
#include "PID.h"
#include "SimpleRobotDrive.h"
#include "math/Pose2D.h"

class PIDDriveController {
 public:
  PIDDriveController(const PIDConfig &xConfig, const PIDConfig &yConfig,
                     const PIDConfig &thetaConfig);

  Pose2D Step(const Pose2D &currentPose, const Pose2D &targetPose) const;
  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const PIDDriveController &controller);

 private:
  PID xPID;
  PID yPID;
  PID thetaPID;
};

#endif  // PIDDRIVE_H