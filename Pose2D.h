#ifndef Pose2D_H
#define Pose2D_H

#include <Arduino.h>

class Pose2D
{
public:
    float x, y, rot;
    Pose2D(float x, float y, float rot);
    friend Print &operator<<(Print &output, const Pose2D &pose); // ChatGPT generated
};

#endif