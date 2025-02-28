#include "VectorRobotDrive.h"

/**
 * Initializes a robot drive, where the drive is based on a NORMALIZED Pose2D Input
 */
VectorRobotDrive::VectorRobotDrive(const MotorSetup motorSetups[], int numMotors)
    : SimpleRobotDrive(motorSetups, numMotors), speedPose(0, 0, 0)
{
}

/**
 * Set motor values based on velocities. Input should be normalized
 *
 * @param speedPose motor velocities
 */
void VectorRobotDrive::Set(const Pose2D &speedPose)
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

/**
 * Return speedPose
 *
 * @return Stored write values for motor velocities
 */
Pose2D VectorRobotDrive::GetVelocity()
{
    return speedPose;
}