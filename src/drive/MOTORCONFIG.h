/**
 * @file MOTORCONFIG.h
 * @brief Defines constants for the 2025 robot.
 *
 * This file contains motor configuration constants, including track width,
 * wheel dimensions, encoder settings, and maximum velocities.
 *
 * @author Aldem Pido
 */

#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#include <cmath>

/**
 * @namespace MotorConstants
 * @brief Contains constants for motor configuration.
 */
namespace MotorConstants {
constexpr float TRACK_WIDTH = 10.0f;  ///< Distance between left and right wheels

// No need to redefine PI, Teensy provides it automatically

constexpr float WHEEL_DIAMETER = 3.25f;                     ///< Diameter of the wheel in inches
constexpr float WHEEL_RADIUS = WHEEL_DIAMETER * 0.5f;       ///< Radius of the wheel
constexpr float WHEEL_CIRCUMFERENCE = PI * WHEEL_DIAMETER;  ///< Circumference of the wheel

constexpr float WHEEL_OFFSET_Y = 2.0f;  ///< Offset distance of the wheel in the Y direction
constexpr float BACK_OFFSET_F = 4.0f;   ///< Back wheel offset

constexpr int RAW_TICKS_PER_REVOLUTION = 3;  ///< Encoder raw ticks per revolution
constexpr int GEAR_RATIO = 34;               ///< Gear ratio of the motor
constexpr long TICKS_PER_REVOLUTION =
    RAW_TICKS_PER_REVOLUTION * GEAR_RATIO;  ///< Computed ticks per revolution
constexpr float IN_PER_TICK =
    WHEEL_CIRCUMFERENCE / TICKS_PER_REVOLUTION;  ///< Distance traveled per encoder tick

constexpr float MAX_VELOCITY = 30.0f;         ///< Maximum velocity in inches per second
constexpr float MAX_ACCELERATION = 30.0f;     ///< Maximum acceleration in inches per second squared
constexpr float MAX_ANGULAR_VELOCITY = 4.0f;  ///< Maximum angular velocity in radians per second
constexpr float MAX_ANGULAR_ACCELERATION =
    10.0f;  ///< Maximum angular acceleration in radians per second squared

constexpr float RAW_MOTOR_RPM_NOLOAD = 11880.0f;  ///< Raw motor RPM without load
constexpr float MOTOR_RPM_NOLOAD =
    RAW_MOTOR_RPM_NOLOAD / GEAR_RATIO;  ///< Adjusted motor RPM accounting for gear ratio
constexpr float MOTOR_RPS_NOLOAD =
    MOTOR_RPM_NOLOAD / 60.0f;  ///< Motor revolutions per second without load

constexpr float DRIVER_START_OFFSET_DEGREES = 90.0f;  ///< Initial driver start offset in degrees
constexpr float DRIVER_START_OFFSET =
    DRIVER_START_OFFSET_DEGREES * PI / 180;  ///< Initial driver start offset in radians
}  // namespace MotorConstants

#endif
