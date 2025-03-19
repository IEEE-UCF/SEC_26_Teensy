#ifndef VECTORROBOTDRIVEPID_H
#define VECTORROBOTDRIVEPID_H

#include "VectorRobotDrive.h"
#include "PIDDriveController.h"

class VectorRobotDrivePID : public VectorRobotDrive
{
public:
    VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors, Print &output, const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig);

    void SetTarget(const Pose2D &targetPose) { this->targetPose = targetPose; }
    void Step() { pidController.Step(localization.getPosition(), this->targetPose); }

private:
    PIDDriveController pidController;
    Pose2D targetPose;
};

#endif // VECTORROBOTDRIVEPID_H