#include "VectorRobotDrivePID.h"

VectorRobotDrivePID::VectorRobotDrivePID(const MotorSetup motorSetups[], int numMotors, Print &output, const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig)
    : targetPose(0, 0, DRIVER_START_OFFSET),
      VectorRobotDrive(motorSetups, numMotors, output),
      pidController(xConfig, yConfig, thetaConfig) {}

void VectorRobotDrivePID::SetTargetByVelocity(const Pose2D &speedPose)
{
  static elapsedMicros callTime = 0;

  // Debug: Print the elapsed time
  Serial.print("callTime (microseconds): ");
  Serial.println(callTime);

  if (callTime <= 1000) // Skip if the elapsed time is too small
  {
    Serial.println("Skipped update - Insufficient callTime");
    return;
  }

  float totTime = callTime * 0.000001f; // Convert microseconds to seconds

  // Debug: Print the calculated total time
  Serial.print("totTime (seconds): ");
  Serial.println(totTime);

  // Debug: Print speedPose before multiplying
  Serial.print("speedPose before scaling: X=");
  Serial.print(speedPose.getX());
  Serial.print(", Y=");
  Serial.print(speedPose.getY());
  Serial.print(", Theta=");
  Serial.println(speedPose.getTheta());

  Pose2D deltaPose = Pose2D(speedPose.getX(), speedPose.getY(), speedPose.getTheta())
                         .multConstant(totTime);

  // Debug: Print the calculated deltaPose
  Serial.print("deltaPose: X=");
  Serial.print(deltaPose.getX());
  Serial.print(", Y=");
  Serial.print(deltaPose.getY());
  Serial.print(", Theta=");
  Serial.println(deltaPose.getTheta());

  targetPose.add(deltaPose).fixTheta();

  // Debug: Print the updated targetPose
  Serial.print("targetPose after update: X=");
  Serial.print(targetPose.getX());
  Serial.print(", Y=");
  Serial.print(targetPose.getY());
  Serial.print(", Theta=");
  Serial.println(targetPose.getTheta());

  callTime = 0; // Reset the timer after updating
}

void VectorRobotDrivePID::PrintInfo(Print &output, bool printConfig) const
{
  output.print(F("SimpleRobotDrive Configuration: "));
  output.print(F("Number of Motors: "));
  output.println(numMotors);

  for (int i = 0; i < numMotors; i++)
  {
    output.print(F("Motor "));
    output.print(i);
    output.print(F(": "));
    motors[i]->PrintInfo(output, printConfig); // Unified logic
  }
}

void VectorRobotDrivePID::PrintLocal(Print &output) const
{
  localization.PrintInfo(output);
  output.print(F("Target Location "));
  output << targetPose;
}