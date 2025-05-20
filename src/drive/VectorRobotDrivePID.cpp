/**
 * @file VectorRobotDrivePID.h
 * @ingroup drives
 * @brief Implements robot drive based on PID control.
 *
 * This class extends `VectorRobotDrive` and uses PID controllers for precise robot motion.
 *
 * @author Aldem Pido
 */

#ifndef VECTORROBOTDRIVEPID_H
#define VECTORROBOTDRIVEPID_H

#include "PIDDriveController.h"
#include "VectorRobotDrive.h"

/**
 * @class VectorRobotDrivePID
 * @ingroup drives
 * @brief Drive system utilizing PID control.
 */
class VectorRobotDrivePID : public VectorRobotDrive {
 public:
  VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors, Print &output,
                      const PIDConfig &xConfig, const PIDConfig &yConfig,
                      const PIDConfig &thetaConfig);

  void SetTarget(const Pose2D &targetPose) { this->targetPose = targetPose; }
  Pose2D Step();
  void SetTargetByVelocity(const Pose2D &speedPose);
  void PrintInfo(Print &output, bool printConfig) const;
  void PrintLocal(Print &output) const;
  void PrintController(Print &output, bool printConfig) const;

 private:
  PIDDriveController pidController;  ///< PID controller for position correction
  Pose2D targetPose;                 ///< Target position for the robot
};

#endif  // VECTORROBOTDRIVEPID_H
