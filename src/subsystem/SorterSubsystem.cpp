#include "SorterSubsystem.h"

SorterSubsystem::SorterSubsystem(int iTOF, int hallCount, int iServo, TOFHandler &tofs, HallHandler &halls, ServoHandler &servos, DriveMotor &transferMotor)
    : iTOF(iTOF),
      hallCount(hallCount),
      iServo(iServo),
      tofs(tofs),
      halls(halls),
      servos(servos),
      transferMotor(transferMotor),
      _state(0),
      _baseReadings(nullptr) {}

void SorterSubsystem::Begin()
{
    halls.Update();
    _baseReadings = halls.getReadings();
    _state = 0;
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
*/

/**
 * Updates, run every frame
 */
void SorterSubsystem::Update()
{
    static elapsedMillis timer = 0;
    static bool objectMagnet = false;
    switch (_state)
    {
    case 0: // no object detected yet.
    {
        if (timer < 1000)
        {
            transferMotor.Set(100); // funnel into sorter
        }
        else if (timer < 1100)
        {
            transferMotor.Set(-50); // clear jams
        }
        else
        {
            timer = 0;
        }
        MoveCenter();                    // write center angle
        int range = tofs.GetIndex(iTOF); // get range reading from tof
        if (range < OBJECT_RANGE)
        {
            _state = 1; // if object is detcted, switch state
            timer = 0;
        }
        break;
    }

    case 1: // object detected. wait a bit
    {
        // Object newly detected
        transferMotor.Set(0); // pause transfer
        if (timer > 500)
        {
            _state = 2;
            timer = 0;
        }
        break;
    }

    case 2: // object detection loop
    {
        objectMagnet = false;                 // object does not have a magnet
        int *_readings = halls.getReadings(); // get halls readings
        for (int i = 0; i < hallCount; i++)
        {
            if (abs(_readings[i] - _baseReadings[i]) > BOUNDS_MAG)
            {
                objectMagnet = true; // object does have a magnet
            }
        }
        _state = 3;
        timer = 0;
        break;
    }

    case 3: // write the correct servo angle. If object leaves, then move on
    {
        if (objectMagnet)
        {
            MoveLeft();
        }
        else
        {
            MoveRight();
        }
        int range = tofs.GetIndex(iTOF);
        if (range > OBJECT_RANGE)
        {
            _state = 4;
            timer = 0;
        }
    }
    break;

    case 4: // wait a bit for object to fall out
    {
        if (timer > 500)
        {
            _state = 0;
            timer = 0;
        }
    }
    }
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

void SorterSubsystem::PrintInfo(Print &output, bool printConfig) const
{
    if (printConfig)
    {
        // Print base readings from Hall sensors
        output.print(F("Base Readings: "));
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
    }
    else
    {
        // Print current state of the subsystem
        output.print(F("SorterSubsystem State: "));
        output.println(_state);
    }
}

#include "SorterSubsystem.h"

// Overloaded operator to print the subsystem state (printConfig = false)
Print &operator<<(Print &output, const SorterSubsystem &subsystem)
{
    subsystem.PrintInfo(output, false); // Use false by default for printConfig
    return output;
}
