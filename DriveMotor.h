#ifndef DriveMotor_h
#define DriveMotor_h

#include "Arduino.h"

#define SPEED_MAX 255 // Max speed input, normalization purposes
#define PWM_MAX 255   // PWM control max

class DriveMotor
{
public:
    DriveMotor(int kPWM, int kCW, int kENC, bool kRev);
    // void Begin();
    void Set(int speed);
    void Read(int *reading);
    void Write();

private:
    int kPWM;
    int kCW;
    int kENC;
    bool kRev;
    int pwmout;
    bool cwout;
};

#endif