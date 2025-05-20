/**
 * @file MandibleSubsystem.h
 * @author Aldem Pido
 * @brief Defines the MandibleSubsystem class for controlling a pair of mandibles.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */

#ifndef MANDIBLESUBSYSTEM_H
#define MANDIBLESUBSYSTEM_H

#include <Arduino.h>

#include "../handler/ServoHandler.h"  // Ensure ServoHandler.h path is correct

/**
 * @class MandibleSubsystem
 * @ingroup subsystems
 * @brief Controls a pair of servo-actuated mandibles (left and right).
 *  Manages the opening and closing of left and right mandibles using
 * predefined servo positions.
 */
class MandibleSubsystem {
 public:
  /**
   * @brief Predefined servo angle positions for the mandibles.
   */
  enum Positions : uint8_t {
    LEFT_OPEN = 180,   ///< Servo angle for the left mandible when fully open.
    LEFT_CLOSE = 115,  ///< Servo angle for the left mandible when fully closed.
    RIGHT_OPEN = 0,    ///< Servo angle for the right mandible when fully open.
    RIGHT_CLOSE = 55   ///< Servo angle for the right mandible when fully closed.
  };

  MandibleSubsystem(int leftServoIndex, int rightServoIndex, ServoHandler &servos);
  void Begin();
  // void Update(); // Commented out in original, kept here for consistency if it might be added
  void OpenLeft();
  void CloseLeft();
  void OpenRight();
  void CloseRight();
  // void SetState(); // Present in .h but not in .cpp, potentially incomplete or for future use.

 private:
  int indexLeft;         ///< Index of the left mandible servo within the ServoHandler.
  int indexRight;        ///< Index of the right mandible servo within the ServoHandler.
  ServoHandler &servos;  ///< Reference to the ServoHandler for controlling the servos.
};

#endif  // MANDIBLESUBSYSTEM_H