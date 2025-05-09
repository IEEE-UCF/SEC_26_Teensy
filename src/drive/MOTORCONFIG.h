/*
Aldem Pido - 4/1/25
MOTORCONFIG.h - define constants for year of 25's robot
*/
#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#include <cmath>

namespace MotorConstants {
constexpr float TRACK_WIDTH = 10.0f;

/*static void SetTrackWidth(float newWidth, Print &output)
{
    constexpr float MIN_TRACK_WIDTH = 5.0f;
    if (fabsf(newWidth) > 1e-6f && newWidth >= MIN_TRACK_WIDTH)
    {
        TRACK_WIDTH = newWidth;
    }
    else
    {
        output.println(F("Error: TRACK_WIDTH must be >= 5.0!"));
    }
}*/

// constexpr float PI = 3.14159265358979323846f; no need. teensy defines this
// automatically
constexpr float WHEEL_DIAMETER = 3.25f;
constexpr float WHEEL_RADIUS = WHEEL_DIAMETER * 0.5f;
constexpr float WHEEL_CIRCUMFERENCE = PI * WHEEL_DIAMETER;

constexpr float WHEEL_OFFSET_Y = 2.0f;
constexpr float BACK_OFFSET_F = 4.0f;

constexpr int RAW_TICKS_PER_REVOLUTION = 3;
constexpr int GEAR_RATIO = 34;
constexpr long TICKS_PER_REVOLUTION = RAW_TICKS_PER_REVOLUTION * GEAR_RATIO;
constexpr float IN_PER_TICK = WHEEL_CIRCUMFERENCE / TICKS_PER_REVOLUTION;

constexpr float MAX_VELOCITY = 30.0f;
constexpr float MAX_ACCELERATION = 30.0f;
constexpr float MAX_ANGULAR_VELOCITY = 4.0f;
constexpr float MAX_ANGULAR_ACCELERATION = 10.0f;

constexpr float RAW_MOTOR_RPM_NOLOAD = 11880.0f;
constexpr float MOTOR_RPM_NOLOAD = RAW_MOTOR_RPM_NOLOAD / GEAR_RATIO;
constexpr float MOTOR_RPS_NOLOAD = MOTOR_RPM_NOLOAD / 60.0f;

constexpr float DRIVER_START_OFFSET_DEGREES = 90.0f;
constexpr float DRIVER_START_OFFSET = DRIVER_START_OFFSET_DEGREES * PI / 180;
}  // namespace MotorConstants
#endif