#include "SorterSubsystem.h"

SortingSubsystem::SortingSubsystem(int iTOF, int *iHalls, int hallCount, int iServo, TOFHandler &tofs, HallHandler &halls, ServoHandler &servos, DriveMotor &transferMotor)
    : iTOF(iTOF),
      iHalls(iHalls),
      hallCount(hallCount),
      iServo(iServo),
      tofs(tofs),
      halls(halls),
      servos(servos),
      transferMotor(transferMotor),
      _state(0),
      _baseReadings(nullptr) {}

void SortingSubsystem::Begin()
{
    halls.Update();
    _baseReadings = halls.getReadings();
    _state = 0;
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
void SortingSubsystem::Update()
{
    static elapsedMillis timer = 0;
<<<<<<< Updated upstream
    static bool mag = false;
=======
    static bool objectMagnet = false;
>>>>>>> Stashed changes
    switch (_state)
    {
    case 0: // no object detected yet.
    {
<<<<<<< Updated upstream
        // No object detected
        transferMotor.Set(100);                       // run the transfer
        servos.WriteServoAngle(iServo, CENTER_ANGLE); // write center angle
        int range = tofs.GetIndex(iTOF);              // get range reading from tof. Update not required as we call it in main
=======
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
        servos.WriteServoAngle(iServo, SortingSubsystem::ServoPositions::CENTER); // write center angle
        int range = tofs.GetIndex(iTOF);                                          // get range reading from tof
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
        servos.WriteServoAngle(iServo, CENTER_ANGLE);
        if (timer > 250)
=======
        if (timer > 500)
>>>>>>> Stashed changes
        {
            _state = 2;
            timer = 0;
        }
        break;
    }

    case 2: // object detection loop
    {
<<<<<<< Updated upstream
        // Evaluation
        transferMotor.Set(0); // pause transfer
        int *_readings = halls.getReadings();
        servos.WriteServoAngle(iServo, CENTER_ANGLE);
        mag = false;
        for (int i = 0; i < hallCount; i++)
        {
            if (abs(_readings[i] - _baseReadings[i]) >= BOUNDS_MAG)
            {
                mag = true;
                break;
=======
        objectMagnet = false;                 // object does not have a magnet
        int *_readings = halls.getReadings(); // get halls readings
        for (int i = 0; i < hallCount; i++)
        {
            if (abs(_readings[i] - _baseReadings[i]) > BOUNDS_MAG)
            {
                objectMagnet = true; // object does have a magnet
>>>>>>> Stashed changes
            }
        }
        _state = 3;
        timer = 0;
        break;
    }

<<<<<<< Updated upstream
    case 3:
    {
        // Operate servo
        transferMotor.Set(0); // pause transfer
        servos.WriteServoAngle(iServo, mag ? LEFT_ANGLE : RIGHT_ANGLE);
        if (timer > 1000)
        {
            _state = 0;
            timer = 0;
        }
    }
=======
    case 3: // write the correct servo angle. If object leaves, then move on
    {
        if (objectMagnet)
        {
            servos.WriteServoAngle(iServo, SortingSubsystem::ServoPositions::LEFT);
        }
        else
        {
            servos.WriteServoAngle(iServo, SortingSubsystem::ServoPositions::RIGHT);
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
>>>>>>> Stashed changes
    }
}

void SortingSubsystem::PrintInfo(Print &output, bool printConfig) const
{
<<<<<<< Updated upstream
    output.println("Sorting Subsystem Info:");
    output.print("TOF Channel: ");
    output.println(iTOF);
    output.print("Servo Channel: ");
    output.println(iServo);
    output.print("Hall Count: ");
    output.println(hallCount);
    output.print("Current State: ");
    output.println(_state);

    if (printConfig)
    {
        output.println("Configuration:");
        output.print("Object Range: ");
        output.println(OBJECT_RANGE);
        output.print("Bounds Magnitude: ");
        output.println(BOUNDS_MAG);
        output.print("Center Angle: ");
        output.println(CENTER_ANGLE);
        output.print("Left Angle: ");
        output.println(LEFT_ANGLE);
        output.print("Right Angle: ");
        output.println(RIGHT_ANGLE);
    }
}

Print &operator<<(Print &output, const SortingSubsystem &subsystem)
{
    subsystem.PrintInfo(output);
    return output;
}
=======
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
        output.print(F("SortingSubsystem State: "));
        output.println(_state);
    }
}

#include "SorterSubsystem.h"

// Overloaded operator to print the subsystem state (printConfig = false)
Print &operator<<(Print &output, const SortingSubsystem &subsystem)
{
    subsystem.PrintInfo(output, false); // Use false by default for printConfig
    return output;
}
>>>>>>> Stashed changes
