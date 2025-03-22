#include "LocalizationEncoder.h"
#include "MOTORCONFIG.h"
#include <Arduino.h>

LocalizationEncoder::LocalizationEncoder()
    : transform(0, 0, 0) {}

/**
 * Update position of the encoder.
 *
 * @param encoderCounts Array of encoder counts, {left, back, right}
 */
void LocalizationEncoder::updatePosition(const long encoderCounts[3], float yaw)
{
    /*if (fabsf(trackWidth) < 1e-6f)
    {
        output.println(F("Error: TRACK_WIDTH invalid!"));
        return;
    }*/

    const long leftChangeTicks = encoderCounts[0] - previousLeftTicks;
    const long backChangeTicks = encoderCounts[1] - previousBackTicks;
    const long rightChangeTicks = encoderCounts[2] - previousRightTicks;
    const float yawChange = yaw - previousYaw;

    previousLeftTicks = encoderCounts[0];
    previousBackTicks = encoderCounts[1];
    previousRightTicks = encoderCounts[2];
    previousYaw = yaw;

    const float leftDistance = leftChangeTicks * IN_PER_TICK;
    const float backDistance = backChangeTicks * IN_PER_TICK;
    const float rightDistance = rightChangeTicks * IN_PER_TICK;

    // const float deltaTheta = (rightDistance - leftDistance) / trackWidth;
    const float cosTheta = cosf(yaw);
    const float sinTheta = sinf(yaw);

    const float deltaX = ((leftDistance + rightDistance) * 0.5f * cosTheta) -
                         (backDistance * sinTheta); // -
                                                    //(WHEEL_OFFSET_Y * deltaTheta * sinTheta); we can assume 100% slippage, this term is irrelevant

    const float deltaY = ((leftDistance + rightDistance) * 0.5f * sinTheta) +
                         (backDistance * cosTheta); // +
                                                    //(WHEEL_OFFSET_Y * deltaTheta * cosTheta); we can assume 100% slippage, this term is irrelevant
                                                    //(MotorConstants::BACK_OFFSET_F * deltaTheta * cosTheta);

    const float deltaTheta = yawChange;
    transform.add(Pose2D(deltaX, deltaY, deltaTheta)).fixTheta();
}

/**
 * Return transform of the robot.
 *
 * @return Transform of robot.
 */
Pose2D LocalizationEncoder::getPosition() const
{
    return transform;
}

/**
 * Override and set position of the robot.
 *
 * @param transform Pose2D of the new position
 */
void LocalizationEncoder::setPosition(const Pose2D &transform)
{
    this->transform = transform;
}

void LocalizationEncoder::PrintInfo(Print &output) const
{
    output.println(F("Localization Encoder Information:"));
    output.print(F("Location "));
    output << transform;
    /*output.print(F("Translation X: "));
    output.println(transform.getX());
    output.print(F("Translation Y: "));
    output.println(transform.getY());
    output.print(F("Translation Theta: "));
    output.println(transform.getTheta());*/
}

Print &operator<<(Print &output, const LocalizationEncoder &encoder)
{
    encoder.PrintInfo(output);
    return output;
}