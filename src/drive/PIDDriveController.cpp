#include "PIDDriveController.h"

/**
 * @brief Constructs a PIDDriveController with individual PID controllers.
 * @param xConfig Configuration for X-axis PID.
 * @param yConfig Configuration for Y-axis PID.
 * @param thetaConfig Configuration for Theta PID.
 */
PIDDriveController::PIDDriveController(const PIDConfig &xConfig, const PIDConfig &yConfig,
                                       const PIDConfig &thetaConfig)
    : xPID(xConfig), yPID(yConfig), thetaPID(thetaConfig) {}

/**
 * @brief Calculates movement correction using PID controllers.
 * @param currentPose Current position of the robot.
 * @param targetPose Desired target position.
 * @return Pose2D containing computed movement corrections.
 */
Pose2D PIDDriveController::Step(const Pose2D &currentPose, const Pose2D &targetPose) const {
  float xSpeed = xPID.Step(currentPose.getX(), targetPose.getX());
  float ySpeed = yPID.Step(currentPose.getY(), targetPose.getY());
  float thetaSpeed = thetaPID.Step(currentPose.getTheta(), targetPose.getTheta());

  Pose2D speedPose = Pose2D(xSpeed, ySpeed, thetaSpeed)
                         .constrainXyMag(MAX_VELOCITY)
                         .constrainTheta(MAX_ANGULAR_VELOCITY);

  Pose2D angleOffsetPose = Pose2D(0, 0, -currentPose.getTheta()).fixTheta();
  Pose2D rotatedSpeedPose = speedPose.rotateVector(angleOffsetPose.getTheta());
  return rotatedSpeedPose;
}

/**
 * @brief Prints PID controller information.
 * @param output Output stream for logging.
 * @param printConfig If true, prints configuration details; otherwise, prints runtime values.
 */
void PIDDriveController::PrintInfo(Print &output, bool printConfig) const {
  output.println(F("PID Drive Controller Info:"));

  output.println(F("X-axis PID:"));
  xPID.PrintInfo(output, printConfig);

  output.println(F("Y-axis PID:"));
  yPID.PrintInfo(output, printConfig);

  output.println(F("Theta PID:"));
  thetaPID.PrintInfo(output, printConfig);
}

/**
 * @brief Overloaded stream operator for printing PID controller details.
 * @param output Output stream.
 * @param controller PIDDriveController instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const PIDDriveController &controller) {
  controller.PrintInfo(output, false);
  return output;
}
