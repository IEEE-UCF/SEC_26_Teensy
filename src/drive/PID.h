#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include <elapsedMillis.h>

struct PIDConfig
{
    double kp;
    double ki;
    double kd;
    double kaw;
    double timeConst;
    double max;
    double min;
    double maxRate;
};

class PID
{
public:
    PID(double kp, double ki, double kd, double kaw, double timeConst, double max, double min, double maxRate)
        : kp(kp), ki(ki), kd(kd), kaw(kaw), timeConst(timeConst), max(max), min(min), maxRate(maxRate) {}
    PID(const PIDConfig &config)
        : kp(config.kp), ki(config.ki), kd(config.kd), kaw(config.kaw), timeConst(config.timeConst), max(config.max), min(config.min), maxRate(config.maxRate) {}
    double Step(double measurement, double setpoint);

private:
    double kp;                 // Proportional gain
    double ki;                 // Integral gain
    double kd;                 // Derivative gain
    double kaw;                // Anti-windup gain
    double timeConst;          // Time constant for derivative filtering
    double timeStepMin = 0.01; // Time step for derivative filtering
    double max;                // Maximum output
    double min;                // Minimum output
    double maxRate;            // Maximum rate of change of output
    double integral = 0;       // Integral term
    double prevError = 0;      // Previous error
    double derivPrev = 0;      // Previous derivative term
    double prevSatCommand = 0; // Previous saturated command
    double prevCommand = 0;    // Previous command
    double satCommand = 0;     // Current saturated command
};
#endif