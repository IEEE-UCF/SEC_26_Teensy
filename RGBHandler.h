#ifndef RGBHandler_h
#define RGBHandler_h

#include "Arduino.h"

class RGBHandler
{
public:
    RGBHandler(int kR = 8, int kG = 7, int kB = 4, bool isAnalog = false);
    void setRGB(int r, int g = -1, int b = -1);
    void clearRGB();

private:
    int _kR;
    int _kG;
    int _kB;
    bool _isAnalog;
};

#endif