#include "VectorRobotDrivePID.h"

/**
 * @brief Constructs a VectorRobotDrivePID object.
 * @param motorSetups Array of motor configurations.
 * @param numMotors Number of motors.
 * @param output Output stream for logging.
 * @param xConfig PID configuration for X-axis.
 * @param yConfig PID configuration for Y-axis.
 * @param thetaConfig PID configuration for rotational control.
 */
VectorRobotDrivePID::VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors,
                                         Print &output, const PIDConfig &xConfig,
                                         const PIDConfig &yConfig, const PIDConfig &thetaConfig)
    : VectorRobotDrive(motorSetups, numMotors, output),
      pidController(xConfig, yConfig, thetaConfig),
      targetPose(0, 0, DRIVER_START_OFFSET) {}

/**
 * @brief Computes a new velocity target based on the current speed.
 * @param speedPose Current velocity pose.
 */
void VectorRobotDrivePID::SetTargetByVelocity(const Pose2D &speedPose) {
  static elapsedMicros callTime = 0;
  float totTime = callTime * 0.000001f;  // Convert microseconds to seconds

  Pose2D deltaPose = Pose2D(speedPose.getX(), speedPose.getY(), speedPose.getTheta())
                         .multConstant(totTime)
                         .multConstant(0.7f);

  targetPose.add(deltaPose).fixTheta();
  callTime = 0;  // Reset the timer after updating
}

/**
 * @brief Computes the correction using PID control to move towards the target pose.
 * @return Pose2D containing the corrected movement.
 */
Pose2D VectorRobotDrivePID::Step() {
  return pidController.Step(localization.getPosition(), targetPose);
}

/**
 * @brief Prints drive configuration and motor details.
 * @param output Output stream for logging.
 * @param printConfig If true, prints motor configuration; otherwise, prints runtime values.
 */
void VectorRobotDrivePID::PrintInfo(Print &output, bool printConfig) const {
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
 * @brief Prints localization information along with the target pose.
 * @param output Output stream for logging.
 */
void VectorRobotDrivePID::PrintLocal(Print &output) const {
  localization.PrintInfo(output);
  output.print(F("Target Location "));
  output << targetPose;
}

/**
 * @brief Prints details of the PID controller.
 * @param output Output stream for logging.
 * @param printConfig If true, prints configuration details; otherwise, prints runtime values.
 */
void VectorRobotDrivePID::PrintController(Print &output, bool printConfig) const {
  output.println(F("PID Controller Details:"));
  pidController.PrintInfo(output, printConfig);
}
