#include "ServoHandler.h"
#include <Arduino.h>

ServoHandler::ServoHandler(int *kServo, int numServos) : numServos(numServos), kServo(kServo) {}

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

Print &operator<<(Print &output, const ServoHandler &handler)
{
    output.println(F("ServoHandler Configuration:"));
    output.print(F("kServo: "));
    for (int i = 0; i < SERVO_COUNT; i++)
    {
        output.print(handler.kServo[i]);
        if (i < SERVO_COUNT - 1)
            output.print(F(", "));
    }
    output.println(); 
    output.println();
}
