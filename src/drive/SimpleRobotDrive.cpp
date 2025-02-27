#include "SimpleRobotDrive.h"
#include <Arduino.h>
#include <Print.h>

SimpleRobotDrive::SimpleRobotDrive(const MotorSetup motorSetups[], int numMotors)
    : numMotors(numMotors), localization()
{
  enc = new int[numMotors];
  motors = new DriveMotor *[numMotors]; // Allocate memory for motor pointers
  for (int i = 0; i < numMotors; i++)
  {
    enc[i] = 0;
    motors[i] = new DriveMotor(motorSetups[i]);
  }
}

void SimpleRobotDrive::Begin()
{
  for (int i = 0; i < numMotors; i++)
  {
    motors[i]->Begin();
  }
}

void SimpleRobotDrive::Set(int motorDirectSpeed[])
{
  for (int i = 0; i < numMotors; i++)
  {
    motors[i]->Set(constrain(motorDirectSpeed[i], -100, 100));
  }
}

void SimpleRobotDrive::SetIndex(int motorDirectSpeed, int index)
{
  if (index >= numMotors)
  {
    Serial.println(F("Motor out of index"));
    return;
  }
  motors[index]->Set(motorDirectSpeed);
}

void SimpleRobotDrive::ReadEnc()
{
  for (int i = 0; i < numMotors; i++)
  {
    motors[i]->ReadEnc();
    enc[i] = motors[i]->GetEnc();
  }
}

void SimpleRobotDrive::ReadAll()
{
  ReadEnc();                        // Update encoder counts
  localization.updatePosition(enc); // Update localization using encoder counts
}

long *SimpleRobotDrive::GetEnc()
{
  return enc;
}

void SimpleRobotDrive::Write()
{
  for (int i = 0; i < numMotors; i++)
  {
    motors[i]->Write();
  }
}

Pose2D SimpleRobotDrive::GetPosition() const
{
  return localization.getPosition();
}

void SimpleRobotDrive::PrintInfo(Print &output, bool printConfig) const
{
  if (printConfig)
  {
    output.print(F("SimpleRobotDrive Configuration: "));
    output.print(F("Number of Motors: "));
    output.println(numMotors);
    for (int i = 0; i < numMotors; i++)
    {
      motors[i]->PrintInfo(output, true);
    }
  }
  else
  {
    for (int i = 0; i < numMotors; i++)
    {
      output.print(F("Motor "));
      output.print(i);
      output.print(F(": "));
      motors[i]->PrintInfo(output, false);
    }
  }
}

void SimpleRobotDrive::PrintLocal(Print &output) const
{
  localization.PrintInfo(output);
}

Print &operator<<(Print &output, const SimpleRobotDrive &drive)
{
  drive.PrintInfo(output, false);
  drive.PrintLocal(output);
  return output;
}
