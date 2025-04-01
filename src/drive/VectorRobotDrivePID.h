/*
Aldem Pido - 4/1/25
VectorRobotDrivePID.h - drive robot based on PID
*/
#ifndef VECTORROBOTDRIVEPID_H
#define VECTORROBOTDRIVEPID_H

#include "VectorRobotDrive.h"
#include "PIDDriveController.h"

class VectorRobotDrivePID : public VectorRobotDrive
{
public:
    VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors, Print &output, const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig);

    void SetTarget(const Pose2D &targetPose) { this->targetPose = targetPose; }
    Pose2D Step() { return pidController.Step(localization.getPosition(), this->targetPose); }
    void SetTargetByVelocity(const Pose2D &speedPose);
    void PrintInfo(Print &output, bool printConfig) const;
    void PrintLocal(Print &output) const;
    void PrintController(Print &output, bool printConfig) const;

private:
    PIDDriveController pidController;
    Pose2D targetPose;
};

#endif // VECTORROBOTDRIVEPID_H