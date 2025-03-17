#include "MandibleSubsystem.h"

MandibleSubsystem::MandibleSubsystem(int indexLeft, int indexRight, ServoHandler &servos)
    : indexLeft(indexLeft),
      indexRight(indexRight),
      servos(servos) {}

void MandibleSubsystem::Begin()
{
    OpenLeft();
    OpenRight();
}

void MandibleSubsystem::OpenLeft()
{
    servos.WriteServoAngle(indexLeft, MandibleSubsystem::Positions::LEFT_OPEN);
}

void MandibleSubsystem::OpenRight()
{
    servos.WriteServoAngle(indexRight, MandibleSubsystem::Positions::RIGHT_OPEN);
}

void MandibleSubsystem::CloseLeft()
{
    servos.WriteServoAngle(indexLeft, MandibleSubsystem::Positions::LEFT_CLOSE);
}

void MandibleSubsystem::CloseRight()
{
    servos.WriteServoAngle(indexRight, MandibleSubsystem::Positions::RIGHT_CLOSE);
}