#ifndef DriveMotor_h
#define DriveMotor_h

#include <Arduino.h>
#include <Print.h>
#include "QuadEncoder.h"

#define SPEED_MAX 255 // Max speed input, normalization purposes
#define PWM_MAX 255   // PWM control max
#define MOTOR_COUNT 4

struct MotorSetup
{
    int kPWM;
    int kCW;
    int kENCA;
    int kENCB;
    bool rev;
};

class DriveMotor
{
public:
    DriveMotor(const MotorSetup &motorSetup);
    void Begin();
    void Set(int speed);
    void ReadEnc();
    long GetEnc();
    void Write();
    void PrintInfo(Print &output, bool printConfig = false) const;
    friend Print &operator<<(Print &output, const DriveMotor &motor);

private:
    MotorSetup motorSetup;
    int pwmout;
    bool cwout;
    long enc;
    QuadEncoder *encoder;
    static int encoderNum;
};

#endif