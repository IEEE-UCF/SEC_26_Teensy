#include "LocalizationEncoder.h"
#include "MOTORCONFIG.h"
#include <Arduino.h>

LocalizationEncoder::LocalizationEncoder() : translation(0, 0, 0)
{
}

void LocalizationEncoder::updatePosition(const long encoderCounts[3])
{
    long leftTicks = encoderCounts[0];
    long rightTicks = encoderCounts[1];
    long backTicks = encoderCounts[2];

    // Calculate the distance each wheel has moved
    float leftDistance = (WHEEL_CIRCUMFERENCE * leftTicks) / TICKS_PER_REVOLUTION;
    float rightDistance = (WHEEL_CIRCUMFERENCE * rightTicks) / TICKS_PER_REVOLUTION;
    float backDistance = (WHEEL_CIRCUMFERENCE * backTicks) / TICKS_PER_REVOLUTION;

    // Calculate the change in orientation (theta)
    float deltaTheta = (rightDistance - leftDistance) / TRACK_WIDTH;

    // Calculate the change in position
    float deltaX = ((leftDistance + rightDistance) / 2) * cos(translation.theta) + backDistance * sin(translation.theta) - WHEEL_OFFSET_Y * deltaTheta * sin(translation.theta);
    float deltaY = ((leftDistance + rightDistance) / 2) * sin(translation.theta) - backDistance * cos(translation.theta) + WHEEL_OFFSET_Y * deltaTheta * cos(translation.theta) + BACK_OFFSET_F * deltaTheta * cos(translation.theta);

    // Update the robot's position
    translation.x += deltaX;
    translation.y += deltaY;
    translation.theta += deltaTheta;
}

Pose2D LocalizationEncoder::getPosition() const
{
    return translation;
}

void LocalizationEncoder::setPosition(const Pose2D &translation)
{
    this->translation = translation;
}

void LocalizationEncoder::PrintInfo(Print &output) const
{
    output.println(F("Localization Encoder Information:"));

    output.print(F("Translation X: "));
    output.println(translation.x);

    output.print(F("Translation Y: "));
    output.println(translation.y);

    output.print(F("Translation Theta: "));
    output.println(translation.theta);

    output.println(F("Encoder Counts:"));
}

Print &operator<<(Print &output, const LocalizationEncoder &encoder)
{
    encoder.PrintInfo(output);
    return output;
}