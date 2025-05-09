/*
Aldem Pido - 4/1/25
SimpleRobotDrive.h - base class for a robot drive
*/
#ifndef SIMPLEROBOTDRIVE_H
#define SIMPLEROBOTDRIVE_H

#include "motor/DriveMotor.h"
#include "LocalizationEncoder.h"
#include <Arduino.h>
#include <Print.h>
#include <vector>
#include <memory>

class SimpleRobotDrive
{
public:
    SimpleRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output);
    void Begin();
    void Set(const int motorDirectSpeed[]);
    void SetIndex(int motorDirectSpeed, int index);
    void ReadAll(float yaw);
    void Write();
    virtual void PrintInfo(Print &output, bool printConfig = false) const;
    virtual void PrintLocal(Print &output) const;
    void SetPosition(const Pose2D &setPosition) { localization.setPosition(setPosition); }
    Pose2D GetPosition() const { return localization.getPosition(); }

protected:
    const int numMotors;
    Print &output;
    elapsedMicros accelCall;
    std::unique_ptr<long[]> enc;
    std::vector<std::unique_ptr<DriveMotor>> motors;
    LocalizationEncoder localization;

    void ReadEnc();
    const long *GetEnc() const;

    friend Print &operator<<(Print &output, const SimpleRobotDrive &drive);
};

Print &operator<<(Print &output, const SimpleRobotDrive &drive);

#endif
