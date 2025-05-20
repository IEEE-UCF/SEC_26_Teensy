/**
 * @file SorterSubsystem.h
 * @author Aldem Pido
 * @brief Defines the SorterSubsystem class for an object sorting mechanism.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */

#ifndef SORTERSUBSYSTEM_H
#define SORTERSUBSYSTEM_H

#include <Arduino.h>
#include <Print.h>  // For Print &operator<<

#include "../drive/DriveMotor.h"      // Ensure path is correct
#include "../handler/HallHandler.h"   // Ensure path is correct
#include "../handler/RGBHandler.h"    // Ensure path is correct
#include "../handler/ServoHandler.h"  // Ensure path is correct
#include "../handler/TOFHandler.h"    // Ensure path is correct

// Constants for sorter operation
#define OBJECT_RANGE 75  ///< TOF sensor range (mm) threshold for detecting an object.
#define BOUNDS_MAG \
  2  ///< Threshold for Hall sensor magnetic detection (specific meaning depends on HallHandler).
#define SORTER_HALL_COUNT 3  ///< Number of Hall effect sensors used by the sorter.

/**
 * @class SorterSubsystem
 * @ingroup subsystems
 * @brief Manages an object sorting mechanism using TOF, Hall sensors, a servo, and a motor.
 *  This subsystem integrates multiple sensors and actuators to detect objects,
 * identify their properties (e.g., magnetic), and sort them using a servo-driven mechanism
 * and a transfer motor.
 */
class SorterSubsystem {
 public:
  /**
   * @brief Predefined servo angle positions for the sorter flap/gate.
   */
  enum ServoPositions : uint8_t {
    LEFT = 40,        ///< Servo angle for sorting to the left.
    SOFTLEFT = 70,    ///< Servo angle for a soft/partial sort to the left.
    CENTER = 90,      ///< Servo angle for the neutral/center position.
    SOFTRIGHT = 110,  ///< Servo angle for a soft/partial sort to the right.
    RIGHT = 140       ///< Servo angle for sorting to the right.
  };

  SorterSubsystem(int tofSensorIndex, int numHallSensors, int servoMotorIndex, TOFHandler &tofs,
                  HallHandler &halls, ServoHandler &servos, DriveMotor &transferMotor,
                  RGBHandler &rgb);
  ~SorterSubsystem();  // Destructor to free _baseReadings

  void Begin();
  void Update();

  void MoveCenter();
  void MoveLeft();
  void MoveRight();
  void MoveSoftLeft();
  void MoveSoftRight();

  void SetState(int newState) {
    this->_state = newState;
  }  ///< Sets the internal state of the sorter.

  void PrintInfo(Print &output, bool printConfig = false) const;
  friend Print &operator<<(Print &output, const SorterSubsystem &subsystem);

 private:
  // Sensor and actuator indices/references
  int iTOF;                   ///< Index of the TOF sensor used for object detection.
  int hallCount;              ///< Number of Hall sensors used (should match SORTER_HALL_COUNT).
  int iServo;                 ///< Index of the sorting servo within ServoHandler.
  TOFHandler &tofs;           ///< Reference to TOFHandler.
  HallHandler &halls;         ///< Reference to HallHandler.
  ServoHandler &servos;       ///< Reference to ServoHandler.
  DriveMotor &transferMotor;  ///< Reference to the DriveMotor for the transfer mechanism.
  RGBHandler &rgb;            ///< Reference to RGBHandler for visual feedback.

  // State variables
  int _state;          ///< Current operational state of the sorter's state machine.
  int *_baseReadings;  ///< Array to store baseline readings from Hall sensors.
  bool objectMagnet;   ///< Flag indicating if the currently detected object is magnetic.
};

#endif  // SORTERSUBSYSTEM_H