/*
Aldem Pido - 4/1/25
VectorRobotDrive.h - drive a robot based on vector velocity
*/
#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "MOTORCONFIG.h"
#include "SimpleRobotDrive.h"
using namespace MotorConstants;

class VectorRobotDrive : public SimpleRobotDrive {
 public:
  VectorRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output);
  Pose2D CalculateRCVector(float x, float y, float theta, float yaw, bool positionControl = false);
  void Set(const Pose2D &speedPose);
  Pose2D GetVelocity() const { return currentSpeedPose; }
  Pose2D GetIdealVelocity() const { return idealSpeedPose; }
  Pose2D ConstrainNewSpeedPose(Pose2D newSpeedPose);

 private:
  Pose2D currentSpeedPose;
  Pose2D idealSpeedPose;
  bool isDeaccelerating(float newValue, float oldValue);
};
#endif