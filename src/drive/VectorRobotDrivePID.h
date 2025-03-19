#ifndef VECTORROBOTDRIVEPID_H
#define VECTORROBOTDRIVEPID_H

#include "VectorRobotDrive.h"
#include "PIDDriveController.h"

class VectorRobotDrivePID : public VectorRobotDrive
{
public:
    VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors, Print &output, const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig);

    void SetTarget(const Pose2D &targetPose) { this->targetPose = targetPose; }
    void StepAndSet() { Set(Step()); }

private:
    PIDDriveController pidController;
    Pose2D targetPose;
    Pose2D Step() { return pidController.Step(localization.getPosition(), this->targetPose); }
};

#endif // VECTORROBOTDRIVEPID_H