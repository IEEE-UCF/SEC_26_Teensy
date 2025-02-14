#include "PoseRobotDrive.h"
#include <Arduino.h>

VectorRobotDrive::VectorRobotDrive(int kPWM[], int kCW[], int kENC[], bool rev[]) : speedPose(0, 0, 0)
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        this->kPWM[i] = kPWM[i];
        this->kCW[i] = kCW[i];
        this->kENC[i] = kENC[i];
        this->rev[i] = rev[i];
        motors[i] = new DriveMotor(kPWM[i], kCW[i], kENC[i], rev[i]);
    }
}

void VectorRobotDrive::Set(const Pose2D &speedPose)
{
    this->speedPose = speedPose;
    // Example logic to set dt. Must use kinematics to convert
    // speedpose into individual wheel speeds
    // motors[i].set(255, true)
}

void VectorRobotDrive::Read(float *arr)
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        float reading;
        motors[i]->Read(&reading);
        arr[i] = reading;
    }
}

void VectorRobotDrive::Write()
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        motors[i]->Write();
    }
}