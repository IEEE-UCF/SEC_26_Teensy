#ifndef DriveHandler_h
#define DriveHandler_h

#include "Arduino.h"

#define MAX_IN 255 //Max value in, negative and positive

class DriveHandler
{
public:
    DriveHandler(int k1, int k2, int k3, int k4, bool rightrev = true, bool dualrev = true);
    void Set(int left, int right, int b1, int b2);
    void Update();

private:
    int kPWM[4];
    int write[4];
    bool _rightrev;
    bool _dualrev;
};

#endif