#include "VectorRobotDrivePID.h"

VectorRobotDrivePID::VectorRobotDrivePID(const MotorSetup motorSetups[],
                                         int numMotors, Print &output,
                                         const PIDConfig &xConfig,
                                         const PIDConfig &yConfig,
                                         const PIDConfig &thetaConfig)
    : VectorRobotDrive(motorSetups, numMotors, output),
      pidController(xConfig, yConfig, thetaConfig),
      targetPose(0, 0, DRIVER_START_OFFSET) {}

void VectorRobotDrivePID::SetTargetByVelocity(const Pose2D &speedPose) {
  static elapsedMicros callTime = 0;
  float totTime = callTime * 0.000001f;  // Convert microseconds to seconds
  Pose2D deltaPose =
      Pose2D(speedPose.getX(), speedPose.getY(), speedPose.getTheta())
          .multConstant(totTime)
          .multConstant(0.7f);
  targetPose.add(deltaPose).fixTheta();
  callTime = 0;  // Reset the timer after updating
}

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

void VectorRobotDrivePID::PrintLocal(Print &output) const {
  localization.PrintInfo(output);
  output.print(F("Target Location "));
  output << targetPose;
}

void VectorRobotDrivePID::PrintController(Print &output,
                                          bool printConfig) const {
  output.println(F("PID Controller Details:"));
  pidController.PrintInfo(output, printConfig);
}
