#include "PID.h"

/**
 * @brief Constructs a PID object with specific gain values.
 * @param kp Proportional gain.
 * @param ki Integral gain.
 * @param kd Derivative gain.
 * @param kaw Integral anti-windup gain.
 * @param timeConst Time constant for derivative filtering.
 * @param max Maximum output value.
 * @param min Minimum output value.
 * @param maxRate Maximum rate of change for output.
 * @param thetaFix Enable angle correction for circular values.
 */
PID::PID(double kp, double ki, double kd, double kaw, double timeConst, double max, double min,
         double maxRate, bool thetaFix)
    : kp(kp),
      ki(ki),
      kd(kd),
      kaw(kaw),
      timeConst(timeConst),
      max(max),
      min(min),
      maxRate(maxRate),
      thetaFix(thetaFix),
      timer(0) {}

/**
 * @brief Constructs a PID object using configuration parameters.
 * @param config PIDConfig struct containing all gain values.
 */
PID::PID(const PIDConfig &config)
    : kp(config.kp),
      ki(config.ki),
      kd(config.kd),
      kaw(config.kaw),
      timeConst(config.timeConst),
      max(config.max),
      min(config.min),
      maxRate(config.maxRate),
      thetaFix(config.thetaFix),
      timer(0) {}

/**
 * @brief Performs a PID step and calculates an output correction.
 * @param measurement The current measured value.
 * @param setpoint The desired target value.
 * @return The computed correction value.
 */
double PID::Step(double measurement, double setpoint) {
  if (timer > timeStepMinSeconds * 1000.0f) {
    double timeStep = timer / 1000000.0f;

    double error = setpoint - measurement;
    if (thetaFix) {
      if (error < -PI)
        error += 2 * PI;
      else if (error > PI)
        error -= 2 * PI;
    }

    integral += (ki * error + kaw * (prevSatCommand - prevCommand)) * timeStep;
    double deriv_filter = (error - prevError + timeConst * derivPrev) / (timeStep + timeConst);
    double command = kp * error + integral + kd * deriv_filter;

    prevCommand = command;
    satCommand = constrain(command, min, max);

    float maxStep = maxRate * timeStep;
    satCommand = constrain(satCommand, prevSatCommand - maxStep, prevSatCommand + maxStep);
    if (abs(satCommand) - abs(prevSatCommand) < 0) {
      float deltaCommand = satCommand - prevSatCommand;
      satCommand += deltaCommand * 2;
    }

    prevSatCommand = satCommand;
    timer = 0;
  }
  return satCommand;
}

/**
 * @brief Prints information about the PID controller.
 * @param output Output stream for logging.
 * @param printConfig If true, prints configuration values; otherwise, prints runtime state.
 */
void PID::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
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
  } else {
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

/**
 * @brief Overloaded stream operator for printing PID state.
 * @param output Output stream.
 * @param pid PID instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const PID &pid) {
  pid.PrintInfo(output, false);
  return output;
}
