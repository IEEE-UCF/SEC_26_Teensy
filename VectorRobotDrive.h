#ifndef PoseRobotDrive_h
#define PoseRobotDrive_h

#include "Pose2D.h"
#include "DriveMotor.h"
#include "Arduino.h"

#define MOTOR_COUNT 4
class VectorRobotDrive

/*
    ASSUME MOTORS ARE IN THE CONFIGURATION:
    left upper, left bottom, right bottom, right upper
*/
{
public:
    VectorRobotDrive(int kPWM[], int kCW[], int kENC[], bool rev[]);
    // void Begin();
    void Set(const Pose2D &speedPose);
    void Read(float *arr);
    void Write();

    friend Print &operator<<(Print &output, const VectorRobotDrive &drive);
    friend Print &operator<<(Print &output, const Pose2D &pose);

private:
    Pose2D speedPose;
    DriveMotor *motors[MOTOR_COUNT];
    // Only used for diagnostics (serial write).
    int kPWM[MOTOR_COUNT];
    int kCW[MOTOR_COUNT];
    int kENC[MOTOR_COUNT];
    bool rev[MOTOR_COUNT];
};

#endif