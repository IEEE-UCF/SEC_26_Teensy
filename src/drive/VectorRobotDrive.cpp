#include "VectorRobotDrive.h"

VectorRobotDrive::VectorRobotDrive(int kPWM[], int kCW[], int kENC_A[], int kENC_B[], bool rev[], int numMotors)
    : SimpleRobotDrive(kPWM, kCW, kENC_A, kENC_B, rev, numMotors), speedPose(0, 0, 0)
{
}

void VectorRobotDrive::Set(const NormalizedPose2D &speedPose)
{
    this->speedPose = speedPose;
    int i = 0;
    while (i < numMotors)
    {
        switch (i)
        {
        case 0:
            motors[i]->Set((speedPose.y - speedPose.theta * TRACK_WIDTH) / WHEEL_DIAMETER * 255);
            break; // Added break statement
        case 1:
            motors[i]->Set(speedPose.x / WHEEL_DIAMETER * 255);
            break; // Added break statement
        case 2:
            motors[i]->Set((speedPose.y + speedPose.theta * TRACK_WIDTH) / WHEEL_DIAMETER * 255);
            break; // Added break statement
        }
        i++;
    }
}

NormalizedPose2D VectorRobotDrive::GetPose()
{
    return speedPose;
}