#include "LineHandler.h"
#include "Arduino.h"

LineHandler::LineHandler(int k1, int k2, int k3, int k4)
{
    kLines[0] = k1;
    kLines[1] = k2;
    kLines[2] = k3;
    kLines[3] = k4;
}

int LineHandler::Get(int i)
{
    return lines[i];
}

void LineHandler::Update()
{
    for (int i = 0; i < 4; i++)
    {
        lines[i] =  analogRead(kLines[i]);
    }
}