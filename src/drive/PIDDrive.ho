#ifndef PIDDRIVE_H
#define PIDDRIVE_H

#include "math/Pose2D.h"
#include "PID.h"
#include <elapsedMillis.h>
#include "MOTORCONFIG.h"
#include "Arduino.h"
class PIDDrive
{
public:
    PIDDrive(const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig, Print &serialOutput);

    Pose2D Step(const Pose2D &currentPose, const Pose2D &targetPose);

private:
    PID xPID;
    PID yPID;
    PID thetaPID;
    Print &serialOutput;
};

#endif // PIDDRIVE_H