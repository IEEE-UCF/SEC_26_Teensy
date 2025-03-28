#include "ServoHandler.h"
#include <Arduino.h>

ServoHandler::ServoHandler(int *kServo, int numServos) : kServo(kServo), numServos(numServos) {}

void ServoHandler::Begin()
{
    servos = new Servo[numServos];
    anglesWrite = new int[numServos];
    for (int i = 0; i < numServos; ++i)
    {
        anglesWrite[i] = -1;
    }
}

void ServoHandler::Attach()
{
    if (attached == nullptr)
    {
        attached = new bool[numServos];
        for (int i = 0; i < numServos; ++i)
        {
            attached[i] = false;
        }
    }

    for (int i = 0; i < numServos; ++i)
    {
        if (!attached[i]) // Only attach servos if they are not attached
        {
            servos[i].attach(kServo[i]);
            attached[i] = true;
        }
    }
}

void ServoHandler::Detach()
{
    for (int i = 0; i < numServos; ++i)
    {
        if (attached[i]) // Only detach servos if they are attached
        {
            servos[i].detach();
            attached[i] = false;
        }
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
