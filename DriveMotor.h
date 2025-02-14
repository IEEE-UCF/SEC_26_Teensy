#ifndef DriveMotor_h
#define DriveMotor_h

#include "Arduino.h"

#define IN_MAX 255 // Max speed input, normalization purposes
#define PWM_MAX 255 // PWM control max

class DriveMotor
{
public:
    DriveMotor(int kPWM, int kCW, int kENC, bool reversed = false);
    void Set(int speed);
    void Read();
    void Update();

private:
    int _kPWM;
    int _kCW;
    int _kENC;
    bool _rev;
    int pwmout;
};

#endif