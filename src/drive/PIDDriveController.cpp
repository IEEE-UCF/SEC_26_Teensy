#include "PIDDriveController.h"

PIDDriveController::PIDDriveController(const PIDConfig &xConfig, const PIDConfig &yConfig,
                                       const PIDConfig &thetaConfig)
    : xPID(xConfig), yPID(yConfig), thetaPID(thetaConfig) {}

Pose2D PIDDriveController::Step(const Pose2D &currentPose, const Pose2D &targetPose) const {
  float xSpeed = xPID.Step(currentPose.getX(), targetPose.getX());
  float ySpeed = yPID.Step(currentPose.getY(), targetPose.getY());
  float thetaSpeed = thetaPID.Step(currentPose.getTheta(), targetPose.getTheta());
  Pose2D speedPose = Pose2D(xSpeed, ySpeed, thetaSpeed)
                         .constrainXyMag(MAX_VELOCITY)
                         .constrainTheta(MAX_ANGULAR_VELOCITY);
  Pose2D angleOffsetPose = Pose2D(0, 0, -currentPose.getTheta()).fixTheta();
  Pose2D rotatedSpeedPose = speedPose.rotateVector(angleOffsetPose.getTheta());
  return rotatedSpeedPose;  // Return the rotated vector
}

// PrintInfo function
void PIDDriveController::PrintInfo(Print &output, bool printConfig) const {
  output.println(F("PID Drive Controller Info:"));

  // Print information for each PID controller
  output.println(F("X-axis PID:"));
  xPID.PrintInfo(output, printConfig);

  output.println(F("Y-axis PID:"));
  yPID.PrintInfo(output, printConfig);

  output.println(F("Theta PID:"));
  thetaPID.PrintInfo(output, printConfig);
}

// Overload << operator
Print &operator<<(Print &output, const PIDDriveController &controller) {
  controller.PrintInfo(output,
                       false);  // Call PrintInfo with printConfig = false
  return output;
}