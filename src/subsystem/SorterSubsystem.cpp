#include "SorterSubsystem.h"

SorterSubsystem::SorterSubsystem(int iTOF, int hallCount, int iServo, TOFHandler &tofs, HallHandler &halls, ServoHandler &servos, DriveMotor &transferMotor, RGBHandler &rgb)
    : iTOF(iTOF),
      hallCount(hallCount),
      iServo(iServo),
      tofs(tofs),
      halls(halls),
      servos(servos),
      transferMotor(transferMotor),
      rgb(rgb),
      _state(0),
      _baseReadings(nullptr),
      objectMagnet(false) {}

void SorterSubsystem::Begin()
{
    halls.Update();
    int *currentReadings = halls.getReadings();
    if (_baseReadings == nullptr)
    {
        _baseReadings = new int[hallCount];
    }

    for (int i = 0; i < hallCount; i++)
    {
        _baseReadings[i] = currentReadings[i];
    }
    _state = 5;
    MoveCenter();
}
/*
State 0:
No object detcted. Run transferMotor

State 1:
Object detected, wait for stabilization.

State 2:
Object detected, detect object type

State 3:
Operate servo until object is out of the way

State 4:
Wait a bit to stabilize

State 5:
Go to 0, stabilize
*/

/**
 * Updates, run every frame
 */
void SorterSubsystem::Update()
{
    int range = tofs.GetIndex(iTOF);
    if (range < OBJECT_RANGE)
    {
        transferMotor.Set(0);
    }
    else
    {
        transferMotor.Set(50);
    }
    transferMotor.Write();
}

void SorterSubsystem::MoveCenter()
{
    servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::CENTER);
}

void SorterSubsystem::MoveLeft()
{
    servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::LEFT);
}

void SorterSubsystem::MoveRight()
{
    servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::RIGHT);
}

void SorterSubsystem::MoveSoftLeft()
{
    servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::SOFTLEFT);
}

void SorterSubsystem::MoveSoftRight()
{
    servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::SOFTRIGHT);
}

void SorterSubsystem::PrintInfo(Print &output, bool printConfig) const
{
    // Print base readings (included in both sections)
    output.print(F("Base Hall Readings: "));
    if (_baseReadings != nullptr)
    {
        for (int i = 0; i < hallCount; i++)
        {
            output.print(_baseReadings[i]);
            if (i < hallCount - 1)
            {
                output.print(F(", "));
            }
        }
    }
    else
    {
        output.print(F("Not initialized"));
    }
    output.println();

    if (printConfig)
    {
        // Config-specific details (add more here if needed)
        // output.println(F("Configuration details here..."));
    }
    else
    {
        // Non-config details
        output.print(F("SorterSubsystem State: "));
        output.print(_state);
        output.print(F(" Detect: "));
        output.println(objectMagnet ? "True" : "False");

        // Print current readings from Hall sensors
        output.print(F("Current Hall Readings: "));
        int *currentReadings = halls.getReadings();
        if (currentReadings != nullptr)
        {
            for (int i = 0; i < hallCount; i++)
            {
                output.print(currentReadings[i]);
                if (i < hallCount - 1)
                {
                    output.print(F(", "));
                }
            }
        }
        else
        {
            output.print(F("Not initialized"));
        }
        output.println();
    }
}

#include "SorterSubsystem.h"

// Overloaded operator to print the subsystem state (printConfig = false)
Print &operator<<(Print &output, const SorterSubsystem &subsystem)
{
    subsystem.PrintInfo(output, false); // Use false by default for printConfig
    return output;
}
