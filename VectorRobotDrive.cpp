#include "VectorRobotDrive.h"
#include <Arduino.h>

VectorRobotDrive::VectorRobotDrive(int kPWM[], int kCW[], int kENC[], bool rev[]) : speedPose(0, 0, 0)
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        this->kPWM[i] = kPWM[i];
        this->kCW[i] = kCW[i];
        this->kENC[i] = kENC[i];
        this->rev[i] = rev[i];
        enc[i] = 0;
        motors[i] = new DriveMotor(kPWM[i], kCW[i], kENC[i], rev[i]);
    }
}

// Ex: robot.Set(Pose2D(x,y,theta))
void VectorRobotDrive::Set(const Pose2D &speedPose)
{
    this->speedPose = speedPose;
    // Example logic to set dt. Must use kinematics to convert
    // speedpose into individual wheel speeds
    // motors[i].set(255, true)
}

void VectorRobotDrive::Read()
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        int reading;
        motors[i]->Read(&reading);
        enc[i] = reading;
    }
}

Pose2D VectorRobotDrive::GetPose()
{
    return speedPose;
}

int *VectorRobotDrive::GetEnc()
{
    return enc;
}

void VectorRobotDrive::Write()
{
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        motors[i]->Write();
    }
}

/*
    ChatGPT generated code
    Chaining multiple Pose2D objects in the main sketch
    Serial << pose1 << pose2 << pose3;
    Serial << robotDrive            prints config
    Serial << robotDrive.speedPose  prints current pose
*/

Print &operator<<(Print &output, const VectorRobotDrive &drive)
{
    output.println(F("VectorRobotDrive Configuration:"));
    output.print(F("kPWM: "));
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        output.print(drive.kPWM[i]);
        if (i < MOTOR_COUNT - 1)
            output.print(F(", "));
    }
    output.println();
    output.print(F("kCW: "));
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        output.print(drive.kCW[i]);
        if (i < MOTOR_COUNT - 1)
            output.print(F(", "));
    }
    output.println();
    output.print(F("kENC: "));
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        output.print(drive.kENC[i]);
        if (i < MOTOR_COUNT - 1)
            output.print(F(", "));
    }
    output.println();
    output.print(F("rev: "));
    for (int i = 0; i < MOTOR_COUNT; i++)
    {
        output.print(drive.rev[i]);
        if (i < MOTOR_COUNT - 1)
            output.print(F(", "));
    }
    output.println();
    output.println();
    return output;
}

/*Print& operator<<(Print& output, const Pose2D& pose) {
    output.print("Current Pose: (");
    output.print(pose.x);
    output.print(", ");
    output.print(pose.y);
    output.print(", ");
    output.print(pose.rot);
    output.println(")");
    return output;
}*/
// defined in pose2d