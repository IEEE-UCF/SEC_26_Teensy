#include "ServoHandler.h"
#include <Arduino.h>

ServoHandler::ServoHandler(int *pins, int numServos) : numServos(numServos), kPins(kPins) {}

void ServoHandler::Setup()
{
    servos = new Servo[numServos];
    anglesWrite = new int[numServos];
    anglesRead = new int[numServos];
    for (int i = 0; i < numServos; ++i)
    {
        servos[i].attach(kPins[i]);
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

void ServoHandler::Read()
{
    anglesRead = anglesWrite;
}
