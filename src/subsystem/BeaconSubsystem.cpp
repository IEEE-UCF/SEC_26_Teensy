#include "BeaconSubsystem.h"

BeaconSubsystem::BeaconSubsystem(int indexBeacon, ServoHandler &servos)
    : indexBeacon(indexBeacon), servos(servos) {}

void BeaconSubsystem::Begin() { MoveUp(); }

void BeaconSubsystem::MoveUp() {
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::UP);
}

void BeaconSubsystem::MoveDown(int offset) {
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN1);
  delay(200);
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN2);
  delay(200);
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN + offset);
}

void BeaconSubsystem::WriteAngle(int angle) {
  angle = constrain(angle, 0, 90);
  servos.WriteServoAngle(indexBeacon, angle);
}