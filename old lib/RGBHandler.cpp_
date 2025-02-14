#include "RGBHandler.h"
#include "Arduino.h"

RGBHandler::RGBHandler(int kR, int kG, int kB, bool isAnalog)
{
    _kR = kR;
    _kG = kG;
    _kB = kB;
    _isAnalog = isAnalog;
    if (!_isAnalog)
    {
        pinMode(_kR, OUTPUT);
        pinMode(_kG, OUTPUT);
        pinMode(_kB, OUTPUT);
    }
}
void RGBHandler::setRGB(int r, int g = -1, int b = -1)
{
    if (g == -1)
    {
        digitalWrite(_kR, (r >> 0) & 1);
        digitalWrite(_kG, (r >> 1) & 1);
        digitalWrite(_kB, (r >> 2) & 1);
        return;
    }
    if (!_isAnalog)
    {
        digitalWrite(_kR, r == 1);
        digitalWrite(_kG, g == 1);
        digitalWrite(_kB, b == 1);
    }
    else
    {
        analogWrite(_kR, r);
        analogWrite(_kG, g);
        analogWrite(_kB, b);
    }
}

void RGBHandler::clearRGB()
{
    if (!_isAnalog)
    {
        digitalWrite(_kR, LOW);
        digitalWrite(_kG, LOW);
        digitalWrite(_kB, LOW);
    }
    else
    {
        analogWrite(_kR, 0);
        analogWrite(_kG, 0);
        analogWrite(_kB, 0);
    }
}
