#include "BeaconSubsystem.h"

BeaconSubsystem::BeaconSubsystem(int indexBeacon, ServoHandler &servos)
    : indexBeacon(indexBeacon),
      servos(servos) {}

void BeaconSubsystem::Begin()
{
    MoveUp();
}

void BeaconSubsystem::MoveUp()
{
    servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::UP);
}

void BeaconSubsystem::MoveDown(int offset)
{
    servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN + offset);
}