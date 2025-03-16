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
Object newly detected.

State 2:
Object identified

State 3:
Operate servo
*/
void SortingSubsystem::Update()
{
    static elapsedMillis timer = 0;
    static bool mag = false;
    switch (_state)
    {
    case 0:
    {
        // No object detected
        transferMotor.Set(100);                       // run the transfer
        servos.WriteServoAngle(iServo, CENTER_ANGLE); // write center angle
        int range = tofs.Get(iTOF);                   // get range reading from tof. Update not required as we call it in main
        if (range < OBJECT_RANGE)
        {
            _state = 1; // if object is detcted, switch state
            timer = 0;
        }
        break;
    }

    case 1:
    {
        // Object newly detected
        transferMotor.Set(0); // pause transfer
        servos.WriteServoAngle(iServo, CENTER_ANGLE);
        if (timer > 250)
        {
            _state = 2;
            timer = 0;
        }
        break;
    }

    case 2:
    {
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
            }
        }
        _state = 3;
        timer = 0;
        break;
    }

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
    }
}