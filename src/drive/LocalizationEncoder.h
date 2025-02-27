#ifndef LOCALIZATIONENCODER_H
#define LOCALIZATIONENCODER_H

#include "math/Pose2D.h"
#include <Arduino.h>

class LocalizationEncoder
{
public:
    LocalizationEncoder();
    void updatePosition(const long encoderCounts[3]);
    Pose2D getPosition() const;
    void setPosition(const Pose2D &translation);
    void PrintInfo(Print &output) const;
    friend Print &operator<<(Print &output, const LocalizationEncoder &translation);

private:
    Pose2D translation;
};

#endif // LOCALIZATIONENCODER_H
