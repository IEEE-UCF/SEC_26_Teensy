#ifndef DriveMotor_h
#define DriveMotor_h

#include <Arduino.h>
#include <Print.h>

#define SPEED_MAX 255 // Max speed input, normalization purposes
#define PWM_MAX 255   // PWM control max
#define MOTOR_COUNT 4
class DriveMotor
{
public:
    DriveMotor(int kPWM, int kCW, int kENC, bool kRev);
    // void Begin();
    void Set(int speed);
    void ReadEnc();
    int GetEnc();
    void Write();
    void PrintInfo(Print &output, bool printConfig = false) const;
    friend Print &operator<<(Print &output, const DriveMotor &motor);


private:
    int kPWM;
    int kCW;
    int kENC;
    bool kRev;
    int pwmout;
    bool cwout;
    int enc;
};

#endif