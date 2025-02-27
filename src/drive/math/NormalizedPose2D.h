#ifndef NormalizedPose2D_H
#define NormalizedPose2D_H

#include "Pose2D.h"
#include <Arduino.h>

class NormalizedPose2D : public Pose2D
{
public:
    NormalizedPose2D(float x, float y, float theta);
    NormalizedPose2D &rotateVector(float angle) override;
    friend Print &operator<<(Print &output, const NormalizedPose2D &pose);

private:
    float constrainValue(float value);
};

#endif
