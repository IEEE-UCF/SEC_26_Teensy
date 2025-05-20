/**
 * @file DriveMotor.cpp
 * @brief Implementation of the DriveMotor class for motor control.
 *
 * This class provides functionality to control a motor using PWM signals and encoders.
 * It supports setting motor speed, reading encoder values, and writing speed to the motor.
 * This also accounts for the weird NFPShop edgecase.
 *
 * @author Aldem Pido
 */

#include "DriveMotor.h"

int DriveMotor::encoderNum = 1;

/**
 * @brief Constructor for DriveMotor.
 * @param motorSetup Configuration settings for the motor.
 * @param output Output stream for logging.
 */
DriveMotor::DriveMotor(const MotorSetup &motorSetup, Print &output)
    : motorSetup(motorSetup), output(output), pwmout(0), cwout(true), enc(0), timeSinceReverse(0) {}

/**
 * @brief Initializes the motor and encoder.
 *
 * Sets up the encoder if available and configures PWM and direction pins.
 */
void DriveMotor::Begin() {
  if (encoderNum <= 4 && motorSetup.kENCA != -1 && motorSetup.kENCB != -1) {
    encoder = std::make_unique<QuadEncoder>(encoderNum, motorSetup.kENCA, motorSetup.kENCB);
    encoderNum++;  ///< Increment only if encoder is created.
    output.println(F("Encoder initialized"));
  } else if (encoderNum > 4) {
    output.println(F("WARNING: Encoder skipped"));
  }

  if (motorSetup.kCW >= 0) pinMode(motorSetup.kCW, OUTPUT);
  if (motorSetup.kPWM >= 0) {
    analogWriteFrequency(motorSetup.kPWM, 18310.55);
    pinMode(motorSetup.kPWM, OUTPUT);
  }

  if (encoder) {
    encoder->setInitConfig();
    encoder->EncConfig.decoderWorkMode = 1;
    encoder->init();
  }

  Set(0);
  Write();
}

/**
 * @brief Sets the motor speed.
 * @param speed Speed value ranging from -255 to 255.
 */
void DriveMotor::Set(int speed) {
  speed = motorSetup.rev ? -speed : speed;
  pwmout = map(abs(speed), 0, SPEED_MAX, PWM_MAX, 0);
  constrain(pwmout, 0, 255);
  cwout = (speed >= 0);
}

/**
 * @brief Reads encoder values and updates internal state.
 */
void DriveMotor::ReadEnc() {
  if (encoder) {
    enc = encoder->read();
    if (!motorSetup.rev) {
      enc = -enc;
    }
  }
}

/**
 * @brief Retrieves the current encoder value.
 * @return Encoder count or 0 if no encoder is available.
 */
long DriveMotor::GetEnc() const {
  return (motorSetup.kENCA == -1 || motorSetup.kENCB == -1) ? 0 : enc;
}

/**
 * @brief Writes the speed to the motor.
 *
 * Handles motor direction and PWM output based on timing constraints. Considers the weird edge case
 * and "reverses" the motor direction slightly periodically.
 */
void DriveMotor::Write() {
  if (timeSinceReverse > 103000) {
    timeSinceReverse = 0;
    digitalWrite(motorSetup.kCW, cwout);
    analogWrite(motorSetup.kPWM, pwmout);
  } else if (timeSinceReverse > 100000) {
    digitalWrite(motorSetup.kCW, !cwout);
    analogWrite(motorSetup.kPWM, 250);
  } else {
    digitalWrite(motorSetup.kCW, cwout);
    analogWrite(motorSetup.kPWM, pwmout);
  }
}

/**
 * @brief Prints motor information.
 * @param output Output stream for logging.
 * @param printConfig If true, prints configuration details; otherwise, prints runtime values.
 */
void DriveMotor::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("DriveMotor Configuration: "));
    output.print(F("kPWM: "));
    output.print(motorSetup.kPWM);
    output.print(F(", kCW: "));
    output.print(motorSetup.kCW);
    output.print(F(", kENC_A: "));
    output.print(motorSetup.kENCA);
    output.print(F(", kENC_B: "));
    output.print(motorSetup.kENCB);
    output.print(F(", kRev: "));
    output.println(motorSetup.rev ? F("True") : F("False"));
  } else {
    output.print(F("DriveMotor PWM Output: "));
    output.print(255 - pwmout);
    output.print(F(", CW Output: "));
    output.print(cwout ? F("True") : F("False"));
    output.print(F(", Encoder: "));
    output.println(enc);
  }
}

/**
 * @brief Overloaded stream operator for printing motor details.
 * @param output Output stream.
 * @param motor DriveMotor instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const DriveMotor &motor) {
  motor.PrintInfo(output, false);
  return output;
}
