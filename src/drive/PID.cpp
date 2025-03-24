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
        integral += (ki * error + kaw * (prevSatCommand - prevCommand)) * timeStep;

        // Calculate filtered derivative
        double deriv_filter = (error - prevError + timeConst * derivPrev) / (timeStep + timeConst);

        // Calculate command using PID equation
        double command = kp * error + integral + kd * deriv_filter;

        // Store previous command
        prevCommand = command;

        // Saturate command
        satCommand = constrain(command, min, max);

        // Apply rate limiter
        float maxStep = maxRate * timeStep;
        satCommand = constrain(satCommand, prevSatCommand - maxStep, prevSatCommand + maxStep);
        if (abs(satCommand) - abs(prevSatCommand) < 0)
        {
            float deltaCommand = satCommand - prevSatCommand;
            satCommand += deltaCommand * 2;
        }

        // Store previous saturated command
        prevSatCommand = satCommand;
        timer = 0;
    }
    return satCommand;
}

void PID::PrintInfo(Print &output, bool printConfig) const
{
    if (printConfig)
    {
        output.println(F("PID Configuration:"));
        output.print(F("Kp: "));
        output.println(kp);
        output.print(F("Ki: "));
        output.println(ki);
        output.print(F("Kd: "));
        output.println(kd);
        output.print(F("Kaw (Anti-Windup): "));
        output.println(kaw);
        output.print(F("Time Constant: "));
        output.println(timeConst);
        output.print(F("Max Output: "));
        output.println(max);
        output.print(F("Min Output: "));
        output.println(min);
        output.print(F("Max Rate: "));
        output.println(maxRate);
        output.print(F("Theta Fix Enabled: "));
        output.println(thetaFix ? "True" : "False");
    }
    else
    {
        output.println(F("PID State:"));
        output.print(F("Integral Term: "));
        output.println(integral);
        output.print(F("Previous Error: "));
        output.println(prevError);
        output.print(F("Previous Derivative: "));
        output.println(derivPrev);
        output.print(F("Previous Saturated Command: "));
        output.println(prevSatCommand);
        output.print(F("Previous Command: "));
        output.println(prevCommand);
        output.print(F("Saturated Command: "));
        output.println(satCommand);
        output.print(F("Timer (Elapsed Microseconds): "));
        output.println(timer);
    }
}

Print &operator<<(Print &output, const PID &pid)
{
    pid.PrintInfo(output, false); // Call PrintInfo with printConfig = false
    return output;
}
