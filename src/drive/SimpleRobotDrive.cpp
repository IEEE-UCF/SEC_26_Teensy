#include "SimpleRobotDrive.h"
#include <Arduino.h>

SimpleRobotDrive::SimpleRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output)
    : numMotors(numMotors > 0 ? numMotors : 1), // Ensure at least 1 motor
      output(output),
      enc(std::make_unique<long[]>(numMotors)),
      localization(output)
{ // Pass output to LocalizationEncoder
    if (numMotors <= 0)
    {
        output.println(F("Error: numMotors must be > 0!"));
        numMotors = 1; // Fallback to prevent crashes
    }

    motors.reserve(numMotors);
    for (int i = 0; i < numMotors; i++)
    {
        motors.emplace_back(std::make_unique<DriveMotor>(motorSetups[i], output));
        enc[i] = 0;
    }
}

/**
 * Begin each motor
 */
void SimpleRobotDrive::Begin()
{
    for (auto &motor : motors)
    {
        motor->Begin();
    }
}

/**
 * Set motor speeds
 * @param motorDirectSpeed array of motors
 */
void SimpleRobotDrive::Set(const int motorDirectSpeed[])
{
    for (int i = 0; i < numMotors; i++)
    {
        motors[i]->Set(constrain(motorDirectSpeed[i], -100, 100));
    }
}

/**
 * Set motor speed by index
 * @param motorDirectSpeed motor speed
 * @param index motor to change
 */
void SimpleRobotDrive::SetIndex(int motorDirectSpeed, int index)
{
    if (index >= numMotors || index < 0)
    {
        output.println(F("Motor index out of bounds"));
        return;
    }
    motors[index]->Set(motorDirectSpeed);
}

/**
 * Read all encoders
 */
void SimpleRobotDrive::ReadEnc()
{
    for (int i = 0; i < numMotors; i++)
    {
        motors[i]->ReadEnc();
        enc[i] = motors[i]->GetEnc();
    }
}

/**
 * Read all encoders and update positions
 */
void SimpleRobotDrive::ReadAll()
{
    ReadEnc();
    localization.updatePosition(enc.get());
}

/**
 * Returns raw encoder values, we never gonna use this lmao
 */
const long *SimpleRobotDrive::GetEnc() const
{
    return enc.get();
}

/**
 * Write to all motors
 */
void SimpleRobotDrive::Write()
{
    for (auto &motor : motors)
    {
        motor->Write();
    }
}

void SimpleRobotDrive::PrintInfo(Print &output, bool printConfig) const
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
