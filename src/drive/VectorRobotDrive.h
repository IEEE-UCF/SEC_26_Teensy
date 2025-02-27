#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "SimpleRobotDrive.h"
#include "math/NormalizedPose2D.h"
#include "MOTORCONST.h"

class VectorRobotDrive : public SimpleRobotDrive
{
public:
    VectorRobotDrive(int kPWM[], int kCW[], int kENC_A[], int kENC_B[], bool rev[], int numMotors);
    void Set(const NormalizedPose2D &speedPose);
    NormalizedPose2D GetPose();

private:
    NormalizedPose2D speedPose;
};

#endif
