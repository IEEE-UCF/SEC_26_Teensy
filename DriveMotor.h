#ifndef DriveMotor_h
#define DriveMotor_h

#include "Arduino.h"

#define SPEED_MAX 255 // Max speed input, normalization purposes
#define PWM_MAX 255   // PWM control max

class DriveMotor
{
public:
    DriveMotor(int kPWM, int kCW, int kENC, bool rev);
    // void Begin();
    void Set(int _speed, bool _fwd);
    void Read(float *reading);
    void Write();

private:
    int kPWM;
    int kCW;
    int kENC;
    bool rev;
    int pwmout;
    bool cwout;
};

#endif