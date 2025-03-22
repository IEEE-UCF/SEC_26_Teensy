#include "PID.h"

/**
 * Step the PID.
 *
 * @param measurement current position
 * @param setpoint set position
 * @return output correction
 */
double PID::Step(double measurement, double setpoint)
{
    if (timer > timeStepMinSeconds * 1000.0f)
    {
        double timeStep = timer / 1000000.0f;
        // Calculate error
        double error = setpoint - measurement;
        if (thetaFix)
        {
            if (error < -PI)
            {
                error += 2 * PI;
            }
            else if (error > PI)
            {
                error -= 2 * PI;
            }
        }

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
            satCommand = max;
        }
        else if (command < min)
        {
            satCommand = min;
        }
        else
        {
            satCommand = command;
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