/**
 * @file DriveMotor.h
 * @brief Defines the DriveMotor class for motor control.
 *
 * This class controls an NFPShop motor using PWM signals and encoders.
 * It accounts for an edge case in the motor controller.
 *
 * @author Aldem Pido
 */

#ifndef DriveMotor_h
#define DriveMotor_h

#include <Arduino.h>
#include <Print.h>

#include <memory>

#include "QuadEncoder.h"

#define SPEED_MAX 255  ///< Maximum speed value
#define PWM_MAX 255    ///< Maximum PWM value

/**
 * @struct MotorSetup
 * @brief Motor configuration settings.
 */
struct MotorSetup {
  int kPWM;   ///< PWM pin for speed control
  int kCW;    ///< Direction control pin
  int kENCA;  ///< Encoder pin A
  int kENCB;  ///< Encoder pin B
  bool rev;   ///< Reverse motor direction flag
};

/**
 * @class DriveMotor
 * @brief Controls a motor using PWM and encoder feedback.
 */
class DriveMotor {
 public:
  explicit DriveMotor(const MotorSetup &motorSetup, Print &output);
  virtual ~DriveMotor() = default;

  void Begin();
  void Set(int speed);
  void ReadEnc();
  long GetEnc() const;
  void Write();
  void PrintInfo(Print &output, bool printConfig = false) const;

  friend Print &operator<<(Print &output, const DriveMotor &motor);

 private:
  MotorSetup motorSetup;                 ///< Motor configuration settings
  Print &output;                         ///< Output stream for logging
  int pwmout;                            ///< PWM output value
  bool cwout;                            ///< Motor direction flag
  long enc;                              ///< Encoder value
  elapsedMicros timeSinceReverse;        ///< Time tracking for motor reversal
  std::unique_ptr<QuadEncoder> encoder;  ///< Encoder instance
  static int encoderNum;                 ///< Static variable to track encoder numbers
};

#endif
