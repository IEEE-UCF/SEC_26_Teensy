/**
 * @file PID.h
 * @brief General-purpose PID controller.
 *
 * Provides a proportional-integral-derivative (PID) control system to adjust
 * output values based on measured error.
 *
 * @author Eren Siegman
 */

#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include <elapsedMillis.h>

/**
 * @struct PIDConfig
 * @brief Stores configuration parameters for the PID controller.
 */
struct PIDConfig {
  double kp;         ///< Proportional correction
  double ki;         ///< Integral correction
  double kd;         ///< Derivative correction
  double kaw;        ///< Integral anti-windup filter
  double timeConst;  ///< Derivative filter constant
  double max;        ///< Maximum output value
  double min;        ///< Minimum output value
  double maxRate;    ///< Maximum rate of change for output
  bool thetaFix;     ///< Enables angle correction for circular values (radians)
};

/**
 * @class PID
 * @brief Implements a PID controller for motion control.
 */
class PID {
 public:
  explicit PID(double kp, double ki, double kd, double kaw, double timeConst, double max,
               double min, double maxRate, bool thetaFix);
  explicit PID(const PIDConfig &config);

  double Step(double measurement, double setpoint);
  void PrintInfo(Print &output, bool printConfig) const;

  friend Print &operator<<(Print &output, const PID &pid);

 private:
  double kp, ki, kd, kaw, timeConst, max, min, maxRate;  ///< PID parameters
  bool thetaFix;                                         ///< Corrects angular values (if needed)
  double integral = 0, prevError = 0, derivPrev = 0;
  double prevSatCommand = 0, prevCommand = 0, satCommand = 0;
  double timeStepMinSeconds = 0.005;
  elapsedMicros timer;
  bool saturated = false;
};

#endif
