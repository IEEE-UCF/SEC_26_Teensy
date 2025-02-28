#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "SimpleRobotDrive.h"
#include "MOTORCONFIG.h"

class VectorRobotDrive : public SimpleRobotDrive
{
public:
    VectorRobotDrive(const MotorSetup motorSetups[], int numMotors);
    void Set(const Pose2D &speedPose);
    Pose2D GetVelocity();

private:
    Pose2D speedPose;
};

#endif
