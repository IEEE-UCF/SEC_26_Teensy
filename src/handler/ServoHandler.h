/**
 * @file ServoHandler.h
 * @author Aldem Pido
 * @brief Defines the ServoHandler class for controlling multiple servo motors.
 * @date April 1, 2025 (as per original comment), Last Modified: May 19, 2025
 * @defgroup actuators Servo Actuator System
 * This group contains components for managing and controlling servo motors,
 * including smooth movement and individual/group control.
 */

#ifndef ServoHandler_h
#define ServoHandler_h

#include <Arduino.h>  // For Print, millis, constrain, etc.
#include <Servo.h>    // Arduino Servo library.

// #include <Print.h> // Already included by Arduino.h for most cores.

// Define constants for servo operation
#define ANGLE_LOW 0     ///< Minimum permissible angle for servos (degrees).
#define ANGLE_HIGH 180  ///< Maximum permissible angle for servos (degrees).
#define DEFAULT_MOVEMENT_SPEED \
  30  ///< Default speed for smooth servo movements (degrees per second).

/**
 * @class ServoHandler
 * @ingroup actuators
 * @brief Manages a collection of servo motors.
 *  Provides functionality to attach, detach, set target angles (immediately or smoothly),
 * and update servo positions. Supports individual and group control of servos.
 * Smooth movement is handled by an `Update()` method that should be called regularly.
 */
class ServoHandler {
 public:
  ServoHandler(int *servoPins, int numServos);  // Changed kServo to servoPins for clarity
  ~ServoHandler();                              // Destructor to free allocated memory

  void Begin();
  void AttachAll();  // Renamed for clarity
  void DetachAll();  // Renamed for clarity

  void SetTargetAngles(const int *targetAnglesArray);  // Renamed for clarity
  void WriteImmediate();  // Renamed for clarity, writes anglesWrite directly
  void SetServoAngleImmediate(int index, int angle);  // Renamed for clarity

  void SetServoAngleSmooth(int index, int angle, int speed = DEFAULT_MOVEMENT_SPEED);
  void UpdateSmoothMovements();    // Renamed for clarity
  void SetGlobalSpeed(int speed);  // Renamed for clarity

  const int *GetCurrentCommandedAngles() const;  // Renamed and made const correct

  void PrintInfo(Print &output, bool printConfig = false) const;
  friend Print &operator<<(Print &output, const ServoHandler &handler);

 private:
  Servo *servos;     ///< Array of Servo objects from the Arduino Servo library.
  int *anglesWrite;  ///< Array storing the current commanded target angles for each servo.
  int *currentPhysicalAngles;   ///< Array storing the current estimated physical angles of servos
                                ///< during smooth moves.
  int *targetSmoothAngles;      ///< Array storing the final target angles for smooth movements.
  unsigned long *lastMoveTime;  ///< Array storing the timestamp of the last movement update for
                                ///< each servo (for smooth moves).
  bool *isAttached;             ///< Array indicating whether each servo is currently attached.

  int *kServoPins;          ///< Array of GPIO pin numbers to which the servos are connected.
  int numServosControlled;  ///< Number of servos managed by this handler.
  int globalMovementSpeed;  ///< Default speed for smooth movements in degrees per second.
};

#endif  // ServoHandler_h