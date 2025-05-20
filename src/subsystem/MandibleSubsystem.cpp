a/**
 * @file MandibleSubsystem.cpp
 * @author Aldem Pido
 * @brief Implements the MandibleSubsystem class for mandible control.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */
#include "MandibleSubsystem.h"

/**
 * @brief Constructs a MandibleSubsystem object.
 *  Initializes the mandible subsystem with the servo indices for the left and
 * right mandibles and a reference to the shared ServoHandler.
 * @param leftServoIndex The index of the left mandible servo in ServoHandler.
 * @param rightServoIndex The index of the right mandible servo in ServoHandler.
 * @param servos Reference to the ServoHandler instance.
 */
MandibleSubsystem::MandibleSubsystem(int leftServoIndex, int rightServoIndex, ServoHandler &servos)
    : indexLeft(leftServoIndex),
      indexRight(rightServoIndex),
      servos(servos) {
}

/**
 * @brief Initializes the mandible subsystem.
 *  Moves both left and right mandibles to their default 'CLOSED' positions.
 */
void MandibleSubsystem::Begin() {
  CloseLeft();
  CloseRight();
}

/**
 * @brief Opens the left mandible.
 *  Commands the left mandible servo to its predefined `LEFT_OPEN` position.
 */
void MandibleSubsystem::OpenLeft() {
  servos.WriteServoAngle(indexLeft, MandibleSubsystem::Positions::LEFT_OPEN);
}

/**
 * @brief Opens the right mandible.
 *  Commands the right mandible servo to its predefined `RIGHT_OPEN` position.
 */
void MandibleSubsystem::OpenRight() {
  servos.WriteServoAngle(indexRight, MandibleSubsystem::Positions::RIGHT_OPEN);
}

/**
 * @brief Closes the left mandible.
 *  Commands the left mandible servo to its predefined `LEFT_CLOSE` position.
 */
void MandibleSubsystem::CloseLeft() {
  servos.WriteServoAngle(indexLeft, MandibleSubsystem::Positions::LEFT_CLOSE);
}

/**
 * @brief Closes the right mandible.
 *  Commands the right mandible servo to its predefined `RIGHT_CLOSE` position.
 */
void MandibleSubsystem::CloseRight() {
  servos.WriteServoAngle(indexRight, MandibleSubsystem::Positions::RIGHT_CLOSE);
}

// Note: The SetState() method declared in MandibleSubsystem.h is not implemented in
// MandibleSubsystem.cpp. If it's intended for use, its implementation should be added.