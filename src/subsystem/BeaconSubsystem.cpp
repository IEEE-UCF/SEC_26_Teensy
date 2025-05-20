/**
 * @file BeaconSubsystem.cpp
 * @author Aldem Pido
 * @brief Implements the BeaconSubsystem class for beacon control.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */
#include "BeaconSubsystem.h"

#include <Arduino.h>  // For delay(), constrain()

/**
 * @brief Constructs a BeaconSubsystem object.
 *  Initializes the beacon subsystem with the specific servo index it controls
 * and a reference to the shared ServoHandler.
 * @param servoIndex The index of the beacon servo managed by the ServoHandler.
 * @param servos Reference to the ServoHandler instance.
 */
BeaconSubsystem::BeaconSubsystem(int servoIndex, ServoHandler &servos)
    : indexBeacon(servoIndex), servos(servos) {}

/**
 * @brief Initializes the beacon subsystem.
 *  Moves the beacon to its default 'UP' position upon initialization.
 */
void BeaconSubsystem::Begin() { MoveUp(); }

/**
 * @brief Moves the beacon to the fully up/deployed position.
 *  Commands the beacon servo to the predefined `UP` angle.
 */
void BeaconSubsystem::MoveUp() {
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::UP);
}

/**
 * @brief Moves the beacon to the fully down/retracted position with an optional offset.
 *  Commands the beacon servo through a sequence of predefined `DOWN1` and `DOWN2`
 * positions with delays, then to the final `DOWN` position plus any specified offset.
 * This staged movement can be for mechanical reasons or smoother operation.
 * @param offset An optional offset angle (in degrees) to add to the final `DOWN` position.
 * Defaults to 0.
 */
void BeaconSubsystem::MoveDown(int offset) {
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN1);
  delay(200);  // Wait for servo to reach intermediate position
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN2);
  delay(200);  // Wait for servo to reach another intermediate position
  servos.WriteServoAngle(indexBeacon, BeaconSubsystem::Positions::DOWN + offset);
}

/**
 * @brief Writes a specific angle to the beacon servo.
 *  Directly commands the beacon servo to a given angle. The angle is constrained
 * between 0 and 90 degrees before being sent to the servo.
 * @param angle The desired angle for the beacon servo (0-90 degrees).
 */
void BeaconSubsystem::WriteAngle(int angle) {
  angle = constrain(angle, 0, 90);  // Constrain angle to a safe/valid range
  servos.WriteServoAngle(indexBeacon, angle);
}