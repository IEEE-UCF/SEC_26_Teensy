/**
 * @file BeaconSubsystem.h
 * @author Aldem Pido
 * @brief Defines the BeaconSubsystem class for controlling a beacon deployment mechanism.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 * @defgroup subsystems Actuator Subsystems
 * This group includes classes that manage specific mechanisms or groups of actuators on the robot.
 */

#ifndef BEACONSUBSYSTEM_H
#define BEACONSUBSYSTEM_H

#include <Arduino.h>

#include "../handler/ServoHandler.h"  // Ensure ServoHandler.h path is correct

/**
 * @class BeaconSubsystem
 * @ingroup subsystems
 * @brief Controls a servo-actuated beacon mechanism.
 * Manages the movement of a beacon, typically to deploy (up) or retract (down) it,
 * using predefined servo positions.
 */
class BeaconSubsystem {
 public:
  /**
   * @brief Predefined servo angle positions for the beacon.
   */
  enum Positions : uint8_t {
    UP = 0,      ///< Servo angle for beacon fully up/deployed.
    DOWN1 = 20,  ///< Intermediate servo angle for first stage of beacon retraction.
    DOWN2 = 40,  ///< Intermediate servo angle for second stage of beacon retraction.
    DOWN = 53    ///< Servo angle for beacon fully down/retracted.
  };

  BeaconSubsystem(int servoIndex, ServoHandler &servos);
  void Begin();
  void MoveUp();
  void MoveDown(int offset = 0);
  void WriteAngle(int angle);

 private:
  int indexBeacon;       ///< Index of the beacon servo within the ServoHandler.
  ServoHandler &servos;  ///< Reference to the ServoHandler for controlling the servo.
};

#endif  // BEACONSUBSYSTEM_H