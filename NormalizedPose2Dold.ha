#ifndef NormalizedPose2D_H
#define NormalizedPose2D_H


#include <Arduino.h>

class NormalizedPose2D
{
public:
    float x, y, rot;
    NormalizedPose2D(float x, float y, float rot);
    friend Print &operator<<(Print &output, const NormalizedPose2D &pose);

private:
    float constrainValue(float value);
};


#endif