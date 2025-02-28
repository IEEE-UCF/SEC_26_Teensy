#include "ServoHandler.h"
#include <Arduino.h>

ServoHandler::ServoHandler(int *kServo, int numServos) : kServo(kServo), numServos(numServos) {}

void ServoHandler::Setup()
{
    servos = new Servo[numServos];
    anglesWrite = new int[numServos];
    for (int i = 0; i < numServos; ++i)
    {
        servos[i].attach(kServo[i]);
        anglesWrite[i] = 0;
    }
}

void ServoHandler::Set(int *anglesWrite)
{
    this->anglesWrite = anglesWrite;
}

void ServoHandler::Write()
{
    for (int i = 0; i < numServos; i++)
    {
        WriteServoAngle(i, anglesWrite[i]);
    }
}

void ServoHandler::WriteServoAngle(int index, int angle)
{
    if (index >= 0 && index < numServos)
    {
        servos[index].write(angle);
        anglesWrite[index] = angle;
    }
}

int *ServoHandler::Get()
{
    return anglesWrite;
}

void ServoHandler::PrintInfo(Print &output, bool printConfig) const
{
    if (printConfig)
    {
        output.print(F("ServoHandler Configuration: "));
        output.print(F("Number of Servos: "));
        output.println(numServos);
        for (int i = 0; i < numServos; i++)
        {
            output.print(F("Servo "));
            output.print(i);
            output.print(F(" - Pin: "));
            output.println(kServo[i]);
        }
    }
    else
    {
        for (int i = 0; i < numServos; i++)
        {
            output.print(F("Servo "));
            output.print(i);
            output.print(F(" - Angle: "));
            output.println(anglesWrite[i]);
        }
    }
}

Print &operator<<(Print &output, const ServoHandler &handler)
{
    handler.PrintInfo(output, false);
    return output;
}
