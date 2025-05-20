/**
 * @file VectorRobotDrive.h
 * @ingroup drives
 * @brief Implements robot drive based on vector velocity.
 *
 * This class extends SimpleRobotDrive and controls robot motion using vector-based velocity inputs.
 *
 * @author Aldem Pido
 */

#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "MOTORCONFIG.h"
#include "SimpleRobotDrive.h"

using namespace MotorConstants;

/**
 * @class VectorRobotDrive
 * @ingroup drives
 * @brief Drive system utilizing vector velocity control.
 */
class VectorRobotDrive : public SimpleRobotDrive {
 public:
  VectorRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output);
  Pose2D CalculateRCVector(float x, float y, float theta, float yaw, bool positionControl = false);
  void Set(const Pose2D &speedPose);
  Pose2D GetVelocity() const { return currentSpeedPose; }
  Pose2D GetIdealVelocity() const { return idealSpeedPose; }
  Pose2D ConstrainNewSpeedPose(Pose2D newSpeedPose);

 private:
  Pose2D currentSpeedPose;  ///< Current speed pose
  Pose2D idealSpeedPose;    ///< Ideal velocity pose
  bool isDeaccelerating(float newValue, float oldValue);
};

#endif
