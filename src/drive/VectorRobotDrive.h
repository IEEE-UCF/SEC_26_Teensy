#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "SimpleRobotDrive.h"
#include "math/NormalizedPose2D.h"
#include "MOTORCONFIG.h"

class VectorRobotDrive : public SimpleRobotDrive
{
public:
    VectorRobotDrive(const MotorSetup motorSetups[], int numMotors);
    void Set(const NormalizedPose2D &speedPose);
    NormalizedPose2D GetPose();

private:
    NormalizedPose2D speedPose;
};

#endif
