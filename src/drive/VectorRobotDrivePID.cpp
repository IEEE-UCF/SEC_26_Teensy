#include "VectorRobotDrivePID.h"

VectorRobotDrivePID::VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors, Print &output, const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig)
    : targetPose(0, 0, 0),
      VectorRobotDrive(motorSetups, numMotors, output),
      pidController(xConfig, yConfig, thetaConfig) {}
