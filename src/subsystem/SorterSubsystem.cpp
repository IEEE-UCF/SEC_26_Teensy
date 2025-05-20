/**
 * @file SorterSubsystem.cpp
 * @author Aldem Pido
 * @brief Implements the SorterSubsystem class for object sorting.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */
#include "SorterSubsystem.h"

#include <Arduino.h>  // For Print, F()

/**
 * @brief Constructs a SorterSubsystem object.
 *  Initializes the sorter with references to various handlers and device indices.
 * Sets initial state values. Does not yet allocate `_baseReadings`.
 * @param tofSensorIndex Index for the TOF sensor in TOFHandler.
 * @param numHallSensors Number of hall sensors (should match SORTER_HALL_COUNT).
 * @param servoMotorIndex Index for the sorting servo in ServoHandler.
 * @param tofs Reference to the TOFHandler.
 * @param halls Reference to the HallHandler.
 * @param servos Reference to the ServoHandler.
 * @param transferMotor Reference to the DriveMotor for the transfer belt/mechanism.
 * @param rgb Reference to the RGBHandler for status indicators.
 */
SorterSubsystem::SorterSubsystem(int tofSensorIndex, int numHallSensors, int servoMotorIndex,
                                 TOFHandler &tofs, HallHandler &halls, ServoHandler &servos,
                                 DriveMotor &transferMotor, RGBHandler &rgb)
    : iTOF(tofSensorIndex),
      hallCount(numHallSensors),  // Should ideally use SORTER_HALL_COUNT if fixed
      iServo(servoMotorIndex),
      tofs(tofs),
      halls(halls),
      servos(servos),
      transferMotor(transferMotor),
      rgb(rgb),
      _state(0),  // Initial state
      _baseReadings(nullptr),
      objectMagnet(false) {}

/**
 * @brief Destructor for SorterSubsystem.
 *  Frees the dynamically allocated `_baseReadings` array.
 */
SorterSubsystem::~SorterSubsystem() { delete[] _baseReadings; }

/**
 * @brief Initializes the sorter subsystem.
 *  Updates Hall sensor readings to establish baseline values.
 * Allocates memory for `_baseReadings` and stores these initial values.
 * Sets the sorter to an initial state (e.g., state 5) and moves the servo to center.
 */
void SorterSubsystem::Begin() {
  halls.Update();                                    // Get initial readings
  const int *currentReadings = halls.getReadings();  // Changed to const int*
  if (_baseReadings == nullptr) {
    _baseReadings = new int[hallCount];
  }

  for (int i = 0; i < hallCount; i++) {
    _baseReadings[i] = currentReadings[i];
  }
  _state = 5;  // Set initial state (e.g., idle or ready)
  MoveCenter();
}

/**
 * @brief Updates the sorter subsystem's logic.
 *  This function is intended to be called repeatedly.
 * It currently checks for object presence using the TOF sensor. If an object is within
 * `OBJECT_RANGE`, it stops the transfer motor; otherwise, it runs the transfer motor
 * at a set speed.
 * The commented-out state machine logic suggests more complex behavior is intended.
 * States:
 * 0: No object detected. Run transferMotor.
 * 1: Object detected by TOF, wait for stabilization.
 * 2: Object stable, detect object type (e.g., using Hall sensors).
 * 3: Object type determined, operate servo to sort. Wait until TOF shows object cleared.
 * 4: Object cleared, wait for system to stabilize (e.g., short delay).
 * 5: Reset sorter to initial position/state (e.g., servo center), ready for next object.
 */
void SorterSubsystem::Update() {
  int range = tofs.GetDistanceAtIndex(iTOF);  // Use GetDistanceAtIndex

  // Basic object detection and transfer motor control
  if (range < OBJECT_RANGE && range != -1) {  // range == -1 might indicate error
    transferMotor.Set(0);                     // Stop motor if object is close
  } else {
    transferMotor.Set(50);  // Run motor if no object or object is far
  }
  transferMotor.Write();  // Apply motor speed

  // State machine logic would typically go here, using _state
  // switch(_state) { ... }
}

/**
 * @brief Moves the sorter servo to the center position.
 *  Commands the sorting servo to the predefined `CENTER` angle.
 */
void SorterSubsystem::MoveCenter() {
  servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::CENTER);
}

/**
 * @brief Moves the sorter servo to the left sorting position.
 *  Commands the sorting servo to the predefined `LEFT` angle.
 */
void SorterSubsystem::MoveLeft() {
  servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::LEFT);
}

/**
 * @brief Moves the sorter servo to the right sorting position.
 *  Commands the sorting servo to the predefined `RIGHT` angle.
 */
void SorterSubsystem::MoveRight() {
  servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::RIGHT);
}

/**
 * @brief Moves the sorter servo to the soft left sorting position.
 *  Commands the sorting servo to the predefined `SOFTLEFT` angle.
 */
void SorterSubsystem::MoveSoftLeft() {
  servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::SOFTLEFT);
}

/**
 * @brief Moves the sorter servo to the soft right sorting position.
 *  Commands the sorting servo to the predefined `SOFTRIGHT` angle.
 */
void SorterSubsystem::MoveSoftRight() {
  servos.WriteServoAngle(iServo, SorterSubsystem::ServoPositions::SOFTRIGHT);
}

/**
 * @brief Prints SorterSubsystem configuration or current state information.
 *  If `printConfig` is true, prints configuration details (currently minimal, focused on
 * base Hall readings). Otherwise, prints runtime state including the sorter's internal state,
 * magnetic detection status, base Hall readings, and current Hall readings.
 * @param output Reference to a Print object (e.g., Serial).
 * @param printConfig Boolean flag; true for configuration, false for current state. Defaults to
 * false.
 */
void SorterSubsystem::PrintInfo(Print &output, bool printConfig) const {
  output.print(F("Base Hall Readings: "));
  if (_baseReadings != nullptr) {
    for (int i = 0; i < hallCount; i++) {
      output.print(_baseReadings[i]);
      if (i < hallCount - 1) {
        output.print(F(", "));
      }
    }
  } else {
    output.print(F("Not initialized"));
  }
  output.println();

  if (printConfig) {
    // output.println(F("Sorter Configuration Details:")); // Example
    // Add more config-specific details if necessary
  } else {
    output.print(F("Sorter State: "));
    output.println(_state);
    output.print(F("Magnetic Object Detected: "));
    output.println(objectMagnet ? "True" : "False");
    output.print(F("TOF Sensor (iTOF: "));
    output.print(iTOF);
    output.print(F(") Reading: "));
    output.println(tofs.GetDistanceAtIndex(iTOF));
    output.print(F("Servo (iServo: "));
    output.print(iServo);
    output.print(F(") Target: "));
    // Assuming ServoHandler has a way to get target angle, e.g.,
    // GetCurrentCommandedAngles()[iServo] If not, this part might need adjustment or removal. For
    // now, we'll omit servo angle to avoid assumptions.
    // output.println(servos.GetCurrentCommandedAngles()[iServo]); // Example, if
    // GetCurrentCommandedAngles() is available and safe
    output.println();

    output.print(F("Current Hall Readings: "));
    const int *currentReadings = halls.getReadings();  // Use const int*
    if (currentReadings != nullptr) {
      for (int i = 0; i < hallCount; i++) {
        output.print(currentReadings[i]);
        if (i < hallCount - 1) {
          output.print(F(", "));
        }
      }
    } else {
      output.print(F("Not available"));
    }
    output.println();
  }
}

/**
 * @brief Overloaded stream insertion operator for printing SorterSubsystem state.
 *  Allows easy printing of the current sorter state to a Print stream.
 * Calls `PrintInfo` with `printConfig = false`.
 * @param output Reference to the Print stream.
 * @param subsystem Const reference to the SorterSubsystem object.
 * @return Reference to the Print stream.
 */
Print &operator<<(Print &output, const SorterSubsystem &subsystem) {
  subsystem.PrintInfo(output, false);
  return output;
}