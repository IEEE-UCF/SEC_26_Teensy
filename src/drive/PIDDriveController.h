/**
 * @file PIDDriveController.h
 * @brief Combines multiple PID controllers for robot motion control.
 *
 * This class fuses X, Y, and Theta PID controllers to manage robot movement.
 *
 * @author Aldem Pido
 */

#ifndef PIDDRIVECONTROLLER_H
#define PIDDRIVECONTROLLER_H

#include "Arduino.h"
#include "MOTORCONFIG.h"
#include "PID.h"
#include "SimpleRobotDrive.h"
#include "math/Pose2D.h"

/**
 * @class PIDDriveController
 * @brief Implements a fused PID controller for robot motion.
 */
class PIDDriveController {
 public:
  PIDDriveController(const PIDConfig &xConfig, const PIDConfig &yConfig,
                     const PIDConfig &thetaConfig);

  Pose2D Step(const Pose2D &currentPose, const Pose2D &targetPose) const;
  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const PIDDriveController &controller);

 private:
  PID xPID;      ///< PID controller for X-axis movement
  PID yPID;      ///< PID controller for Y-axis movement
  PID thetaPID;  ///< PID controller for rotational movement
};

#endif  // PIDDRIVECONTROLLER_H
