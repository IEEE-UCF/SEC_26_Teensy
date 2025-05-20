#include "SimpleRobotDrive.h"

#include <Arduino.h>

/**
 * @brief Constructs a SimpleRobotDrive object.
 * @param motorSetups Array of motor configurations.
 * @param numMotors Number of motors to initialize.
 * @param output Output stream for logging.
 */
SimpleRobotDrive::SimpleRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output)
    : numMotors(numMotors > 0 ? numMotors : 1),
      output(output),
      enc(std::make_unique<long[]>(numMotors)),
      localization() {
  if (numMotors <= 0) {
    output.println(F("Error: numMotors must be > 0!"));
    numMotors = 1;  // Fallback to prevent crashes
  }

  motors.reserve(numMotors);
  for (int i = 0; i < numMotors; i++) {
    motors.emplace_back(std::make_unique<DriveMotor>(motorSetups[i], output));
    enc[i] = 0;
  }
}

/**
 * @brief Initializes all motors.
 */
void SimpleRobotDrive::Begin() {
  for (auto &motor : motors) {
    motor->Begin();
  }
}

/**
 * @brief Sets speed for all motors.
 * @param motorDirectSpeed Array containing speed values for each motor.
 */
void SimpleRobotDrive::Set(const int motorDirectSpeed[]) {
  for (int i = 0; i < numMotors; i++) {
    motors[i]->Set(constrain(motorDirectSpeed[i], -100, 100));
  }
}

/**
 * @brief Sets speed for a specific motor.
 * @param motorDirectSpeed Speed value to set.
 * @param index Index of the motor to update.
 */
void SimpleRobotDrive::SetIndex(int motorDirectSpeed, int index) {
  if (index >= numMotors || index < 0) {
    output.println(F("Motor index out of bounds"));
    return;
  }
  motors[index]->Set(motorDirectSpeed);
}

/**
 * @brief Reads encoder values for all motors.
 */
void SimpleRobotDrive::ReadEnc() {
  for (int i = 0; i < numMotors; i++) {
    motors[i]->ReadEnc();
    enc[i] = motors[i]->GetEnc();
  }
}

/**
 * @brief Reads encoder values and updates localization.
 * @param yaw Current gyro yaw reading.
 */
void SimpleRobotDrive::ReadAll(float yaw) {
  ReadEnc();
  localization.updatePosition(enc.get(), yaw);
}

/**
 * @brief Retrieves raw encoder values.
 * @return Pointer to an array of encoder values.
 */
const long *SimpleRobotDrive::GetEnc() const { return enc.get(); }

/**
 * @brief Sends updated speed values to all motors.
 */
void SimpleRobotDrive::Write() {
  for (auto &motor : motors) {
    motor->Write();
  }
}

/**
 * @brief Prints drive configuration and motor details.
 * @param output Output stream for logging.
 * @param printConfig If true, prints motor configuration; otherwise, prints runtime values.
 */
void SimpleRobotDrive::PrintInfo(Print &output, bool printConfig) const {
  output.print(F("SimpleRobotDrive Configuration: "));
  output.print(F("Number of Motors: "));
  output.println(numMotors);

  for (int i = 0; i < numMotors; i++) {
    output.print(F("Motor "));
    output.print(i);
    output.print(F(": "));
    motors[i]->PrintInfo(output, printConfig);
  }
}

/**
 * @brief Prints localization information.
 * @param output Output stream for logging.
 */
void SimpleRobotDrive::PrintLocal(Print &output) const { localization.PrintInfo(output); }

/**
 * @brief Overloaded stream operator for printing drive details.
 * @param output Output stream.
 * @param drive SimpleRobotDrive instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const SimpleRobotDrive &drive) {
  drive.PrintInfo(output, false);
  drive.PrintLocal(output);
  return output;
}
