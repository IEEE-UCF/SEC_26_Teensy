#include "DriveMotor.h"

int DriveMotor::encoderNum = 1;

DriveMotor::DriveMotor(const MotorSetup &motorSetup, Print &output)
    : motorSetup(motorSetup), output(output), pwmout(0), cwout(true), enc(0), timeSinceReverse(0) {}

/**
 * Begin drivemotor
 */
void DriveMotor::Begin()
{
    if (encoderNum <= 4 && motorSetup.kENCA != -1 && motorSetup.kENCB != -1)
    {
        encoder = std::make_unique<QuadEncoder>(encoderNum, motorSetup.kENCA, motorSetup.kENCB);
        encoderNum++; // Only increment if encoder is created
        output.println(F("Encoder initialized"));
    }
    else if (encoderNum > 4)
    {
        output.println(F("WARNING: Encoder skipped"));
    }
    if (motorSetup.kCW >= 0)
        pinMode(motorSetup.kCW, OUTPUT);
    if (motorSetup.kPWM >= 0)
    {
        analogWriteFrequency(motorSetup.kPWM, 18310.55);
        pinMode(motorSetup.kPWM, OUTPUT);
    }
    if (encoder)
    {
        encoder->setInitConfig();
        encoder->EncConfig.decoderWorkMode = 1;
        encoder->init();
    }
    Set(0);
    Write();
}

/**
 * Set speed of motors from -255 to 255 (PWM_MAX)
 * @param speed pwmout
 */
void DriveMotor::Set(int speed)
{
    speed = motorSetup.rev ? -speed : speed;
    pwmout = map(abs(speed), 0, SPEED_MAX, PWM_MAX, 0);
    constrain(pwmout, 0, 255);
    cwout = (speed >= 0);
}

/**
 * Read encoder values
 */
void DriveMotor::ReadEnc()
{
    if (encoder)
    {
        enc = encoder->read();
        if (!motorSetup.rev)
        {
            enc = -enc;
        }
    }
}

/**
 * Get encoder values
 */
long DriveMotor::GetEnc() const
{
    return (motorSetup.kENCA == -1 || motorSetup.kENCB == -1) ? 0 : enc;
}

/**
 * Write speed to motor
 */
void DriveMotor::Write()
{
    if (timeSinceReverse > 103000)
    {
        timeSinceReverse = 0;
        digitalWrite(motorSetup.kCW, cwout);
        analogWrite(motorSetup.kPWM, pwmout);
    }
    else if (timeSinceReverse > 100000)
    {
        digitalWrite(motorSetup.kCW, !cwout);
        analogWrite(motorSetup.kPWM, 250);
    }
    else
    {
        digitalWrite(motorSetup.kCW, cwout);
        analogWrite(motorSetup.kPWM, pwmout);
    }
    /*if (timeSinceReverse > 10030)
    {
        timeSinceReverse = 0;
    }
    else if (timeSinceReverse > 10000)
    {
        // analogWrite(motorSetup.kPWM, 255);
    }
    else
    {
        analogWrite(motorSetup.kPWM, pwmout);
    }
    if (motorSetup.kPWM >= 0)
    {
        digitalWrite(motorSetup.kCW, cwout);
    }*/
}

void DriveMotor::PrintInfo(Print &output, bool printConfig) const
{
    if (printConfig)
    {
        output.print(F("DriveMotor Configuration: "));
        output.print(F("kPWM: "));
        output.print(motorSetup.kPWM);
        output.print(F(", kCW: "));
        output.print(motorSetup.kCW);
        output.print(F(", kENC_A: "));
        output.print(motorSetup.kENCA);
        output.print(F(", kENC_B: "));
        output.print(motorSetup.kENCB);
        output.print(F(", kRev: "));
        output.println(motorSetup.rev ? F("True") : F("False"));
    }
    else
    {
        output.print(F("DriveMotor PWM Output: "));
        output.print(255 - pwmout);
        output.print(F(", CW Output: "));
        output.print(cwout ? F("True") : F("False"));
        output.print(F(", Encoder: "));
        output.println(enc);
    }
}

Print &operator<<(Print &output, const DriveMotor &motor)
{
    motor.PrintInfo(output, false);
    return output;
}
