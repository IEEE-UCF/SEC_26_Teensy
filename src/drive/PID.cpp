#include "PID.h"

double PID::step(double measurement, double setpoint)
{
    static elapsedMillis timer = 0;
    if (timer > timeStepMin * 1000)
    {
        double timeStep = timer / 1000;
        // Calculate error
        double error = setpoint - measurement;

        // Update integral term with anti-windup
        integral += ki * error * timeStep + kaw * (prevSatCommand - prevCommand) * timeStep;

        // Calculate filtered derivative
        double deriv_filter = (error - prevError + timeConst * derivPrev) / (timeStep + timeConst);

        // Calculate command using PID equation
        double command = kp * error + integral + kd * deriv_filter;

        // Store previous command
        prevCommand = command;

        // Saturate command
        if (command > max)
        {
            command = max;
        }
        else if (command < min)
        {
            command = min;
        }

        // Apply rate limiter
        if (satCommand > prevSatCommand + maxRate * timeStep)
        {
            satCommand = prevSatCommand + maxRate * timeStep;
        }
        else if (satCommand < prevSatCommand - maxRate * timeStep)
        {
            satCommand = prevSatCommand - maxRate * timeStep;
        }

        // Store previous saturated command
        prevSatCommand = satCommand;
        timer = 0;
    }
    return satCommand;
}