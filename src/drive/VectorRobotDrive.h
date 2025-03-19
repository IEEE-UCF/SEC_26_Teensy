#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "SimpleRobotDrive.h"
#include "MOTORCONFIG.h"
using namespace MotorConstants;
class VectorRobotDrive : public SimpleRobotDrive
{
public:
    VectorRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output);
    Pose2D CalculateRCVector(float x, float y, float theta, float yaw);
    void Set(const Pose2D &speedPose);
    Pose2D GetVelocity() const { return currentSpeedPose; }

private:
    Pose2D currentSpeedPose;
    void accelerationConstraint(Pose2D newPose);
    bool isDeaccelerating(float newValue, float oldValue);
};
#endif