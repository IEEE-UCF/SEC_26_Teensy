#ifndef MPUHandler_h
#define MPUHandler_h

#include "Arduino.h"
#include "RGBHandler.h"
#include "Wire.h"
#include "MPU6050_light.h"
#include "TOFHandler.h"

class MPUHandler
{
public:
    MPUHandler(int k) : mpu(Wire) { channel = k; };
    void Begin(RGBHandler rgb);
    void Update();
    int Get(int dimension);

private:
    MPU6050 mpu;
    int channel;
    byte _status;
    int angles[3];
};
#endif