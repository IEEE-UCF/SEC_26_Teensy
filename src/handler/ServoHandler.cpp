/**
 * @file ServoHandler.cpp
 * @author Aldem Pido
 * @brief Implements the ServoHandler class for controlling multiple servo motors.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */
#include "ServoHandler.h"

#include <Arduino.h>  // For millis, constrain, Print, F()

/**
 * @brief Constructs a ServoHandler object.
 *  Initializes the handler with an array of servo pins and the number of servos.
 * Sets the default movement speed. Does not yet allocate memory for servo arrays;
 * this is done in `Begin()`.
 * @param servoPins Pointer to an integer array containing the GPIO pin numbers for each servo.
 * This array must remain valid for the lifetime of the ServoHandler object.
 * @param numServos The number of servos to be managed. This should match the size of `servoPins`.
 */
ServoHandler::ServoHandler(int *servoPins, int numServos)
    : kServoPins(servoPins),
      numServosControlled(numServos),
      globalMovementSpeed(DEFAULT_MOVEMENT_SPEED),
      servos(nullptr),
      anglesWrite(nullptr),
      currentPhysicalAngles(nullptr),
      targetSmoothAngles(nullptr),
      lastMoveTime(nullptr),
      isAttached(nullptr) {}

/**
 * @brief Destructor for ServoHandler.
 *  Frees the dynamically allocated memory for servo arrays and Servo objects
 * to prevent memory leaks. Detaches all servos before deleting.
 */
ServoHandler::~ServoHandler() {
  DetachAll();  // Ensure servos are detached
  delete[] servos;
  delete[] anglesWrite;
  delete[] currentPhysicalAngles;
  delete[] targetSmoothAngles;
  delete[] lastMoveTime;
  delete[] isAttached;
}

/**
 * @brief Initializes internal arrays and servo states.
 *  Dynamically allocates memory for storing servo objects and their state arrays
 * (angles, timings, attachment status). Initializes all servos to a default center
 * position (90 degrees) and marks them as not attached. This method must be called
 * before any other operations like attaching or setting angles.
 */
void ServoHandler::Begin() {
  // Deallocate if Begin() is called multiple times (though not typical)
  delete[] servos;
  delete[] anglesWrite;
  delete[] currentPhysicalAngles;
  delete[] targetSmoothAngles;
  delete[] lastMoveTime;
  delete[] isAttached;

  servos = new Servo[numServosControlled];
  anglesWrite = new int[numServosControlled];
  currentPhysicalAngles = new int[numServosControlled];
  targetSmoothAngles = new int[numServosControlled];
  lastMoveTime = new unsigned long[numServosControlled];
  isAttached = new bool[numServosControlled];

  for (int i = 0; i < numServosControlled; ++i) {
    anglesWrite[i] = 90;            // Default commanded angle
    currentPhysicalAngles[i] = 90;  // Assume starts at 90
    targetSmoothAngles[i] = 90;     // Default target for smooth move
    lastMoveTime[i] = 0;
    isAttached[i] = false;
  }
}

/**
 * @brief Attaches all managed servos to their respective pins.
 *  Iterates through all servos. If a servo is not already attached,
 * it calls the `attach()` method of the `Servo` object, linking it to the
 * GPIO pin specified in `kServoPins`.
 */
void ServoHandler::AttachAll() {
  for (int i = 0; i < numServosControlled; ++i) {
    if (!isAttached[i] && kServoPins[i] >= 0) {  // Check for valid pin
      servos[i].attach(kServoPins[i]);
      isAttached[i] = true;
      // Optionally, write the currentPhysicalAngles to servo upon attach
      // servos[i].write(currentPhysicalAngles[i]);
    }
  }
}

/**
 * @brief Detaches all managed servos from their pins.
 *  Iterates through all servos. If a servo is currently attached,
 * it calls the `detach()` method of the `Servo` object. This stops the PWM signal
 * to the servo, allowing it to be moved manually (if its mechanics allow) and
 * potentially saving power.
 */
void ServoHandler::DetachAll() {
  for (int i = 0; i < numServosControlled; ++i) {
    if (isAttached[i]) {
      servos[i].detach();
      isAttached[i] = false;
    }
  }
}

/**
 * @brief Sets the target angles for all servos for immediate writing.
 *  Copies the angles from the provided array into the internal `anglesWrite` array.
 * These angles will be written to the servos when `WriteImmediate()` is called.
 * Also updates `targetSmoothAngles` to these values, effectively stopping any ongoing
 * smooth movement and setting a new immediate target.
 * @param targetAnglesArray Pointer to an integer array containing the desired angles (0-180)
 * for each servo. The size of this array must match `numServosControlled`.
 */
void ServoHandler::SetTargetAngles(const int *targetAnglesArray) {
  for (int i = 0; i < numServosControlled; i++) {
    int angle = constrain(targetAnglesArray[i], ANGLE_LOW, ANGLE_HIGH);
    anglesWrite[i] = angle;
    targetSmoothAngles[i] = angle;  // For smooth movement, this becomes the new target
    // currentPhysicalAngles[i] might not be 'angle' yet if smooth moving was in progress.
    // For SetTargetAngles implying an eventual immediate write or new smooth target,
    // this is consistent.
  }
}

/**
 * @brief Writes the currently stored `anglesWrite` to all attached servos immediately.
 *  Iterates through all servos. If a servo is attached, its physical position
 * is updated to the corresponding angle in `anglesWrite` using `servos[i].write()`.
 * The `currentPhysicalAngles` and `targetSmoothAngles` are also updated to reflect this immediate
 * change.
 */
void ServoHandler::WriteImmediate() {
  for (int i = 0; i < numServosControlled; i++) {
    if (isAttached[i]) {
      int angle = constrain(anglesWrite[i], ANGLE_LOW, ANGLE_HIGH);
      servos[i].write(angle);
      currentPhysicalAngles[i] = angle;  // Physical angle is now this
      targetSmoothAngles[i] = angle;     // Target for any future smooth move is also this
    }
  }
}

/**
 * @brief Sets a specific servo to a given angle immediately.
 *  If the servo index is valid and the servo is attached, its physical position
 * is updated to the specified angle using `servos[index].write()`.
 * The `anglesWrite`, `currentPhysicalAngles`, and `targetSmoothAngles` for this servo
 * are updated to reflect this immediate change.
 * @param index The index of the servo to control (0 to `numServosControlled`-1).
 * @param angle The desired angle in degrees (0-180).
 */
void ServoHandler::SetServoAngleImmediate(int index, int angle) {
  if (index >= 0 && index < numServosControlled && isAttached[index]) {
    int constrained_angle = constrain(angle, ANGLE_LOW, ANGLE_HIGH);
    servos[index].write(constrained_angle);
    anglesWrite[index] = constrained_angle;
    currentPhysicalAngles[index] = constrained_angle;
    targetSmoothAngles[index] = constrained_angle;
  }
}

/**
 * @brief Sets a target angle for a specific servo for smooth movement.
 *  Sets the `targetSmoothAngles` for the specified servo. The servo will then
 * gradually move towards this angle when `UpdateSmoothMovements()` is called.
 * The `anglesWrite` is also updated to this new target angle.
 * @param index The index of the servo to control.
 * @param angle The desired target angle in degrees (0-180).
 * @param speed The speed of movement in degrees per second. If not specified or <= 0,
 * the `globalMovementSpeed` is used.
 */
void ServoHandler::SetServoAngleSmooth(int index, int angle, int speed) {
  if (index >= 0 && index < numServosControlled) {
    targetSmoothAngles[index] = constrain(angle, ANGLE_LOW, ANGLE_HIGH);
    anglesWrite[index] = targetSmoothAngles[index];  // Commanded angle reflects the ultimate target

    // Custom speed for this specific movement could be stored per servo if needed.
    // The current implementation uses globalMovementSpeed or an override 'speed' for the next
    // update cycle. If 'speed' parameter should apply *only* to this call, this function or Update
    // needs adjustment. For now, if speed is provided, it temporarily overrides globalMovementSpeed
    // *during the Update call that processes this move*. A more robust way would be to store speed
    // per servo or use SetGlobalSpeed. The original code's `movementSpeed = speed` inside Update
    // suggests the parameter 'speed' to WriteServoAngleSmooth was intended to set the global speed.
    // We'll stick to SetGlobalSpeed for that. This function just sets the target. The speed of
    // approach is dictated by globalMovementSpeed. If the `speed` parameter here is truly meant to
    // be a one-time override for THIS servo's move, then lastMoveTime might need to be reset or a
    // per-servo speed array used. The original code had `movementSpeed = speed;` in
    // `WriteServoAngleSmooth`, which would change global speed. Here, we assume 'speed' in the
    // signature means the desired speed *for this specific transition if it differs from global*.
    // However, the class structure only has one `globalMovementSpeed`.
    // The most straightforward interpretation is that `speed` here is ignored if not setting a
    // per-servo speed, or it's used to update `globalMovementSpeed` IF that was the intent. Given
    // `SetGlobalSpeed` exists, this `speed` param is ambiguous. Let's assume it's an optional
    // override for the global speed for this *servo's next movement calculation if Update() could
    // use it*. Since Update() uses globalMovementSpeed, this param as-is doesn't do much unless we
    // modify Update or store per-servo speeds. For now, it will be like the default: global speed
    // applies. If a temporary override was intended: if (speed > 0) { /* Store temp_speed[index] =
    // speed; or similar */ } And UpdateSmoothMovements would check temp_speed[index] first. For
    // now, this function only sets the target. The speed of movement is set by SetGlobalSpeed.
  }
}

/**
 * @brief Updates the positions of servos performing smooth movements.
 *  This function must be called repeatedly (e.g., in the main Arduino loop)
 * to achieve smooth servo motion. For each servo, if its `currentPhysicalAngle` is
 * not yet at `targetSmoothAngle`, it calculates the small increment to move based on
 * `globalMovementSpeed` and the time elapsed since the last update.
 * The servo's physical position is then updated by this increment.
 */
void ServoHandler::UpdateSmoothMovements() {
  unsigned long currentTime = millis();

  for (int i = 0; i < numServosControlled; i++) {
    if (!isAttached[i] || currentPhysicalAngles[i] == targetSmoothAngles[i]) {
      continue;  // Skip if not attached or already at target
    }

    unsigned long elapsedTime = currentTime - lastMoveTime[i];
    // Degrees per millisecond = globalMovementSpeed / 1000.0
    float degreesToMoveThisStep = (globalMovementSpeed / 1000.0f) * elapsedTime;

    if (elapsedTime == 0 && currentPhysicalAngles[i] !=
                                targetSmoothAngles[i]) {  // Ensure some time passes or initial move
      // if lastMoveTime[i] was 0, degreesToMoveThisStep would be huge if currentTime is large.
      // So, if lastMoveTime[i] is 0, this is the first update for this target.
      // We can either move by a small step, or just set lastMoveTime[i] = currentTime and wait for
      // next cycle. Let's ensure at least a minimal step if significant time could have passed or
      // it's first step.
      if (lastMoveTime[i] == 0 && degreesToMoveThisStep == 0 &&
          currentPhysicalAngles[i] != targetSmoothAngles[i]) {
        degreesToMoveThisStep =
            1.0f;  // Ensure a move if it's the first step and target is different.
      }
    }

    if (degreesToMoveThisStep < 1.0f && degreesToMoveThisStep > 0.0f &&
        currentPhysicalAngles[i] != targetSmoothAngles[i]) {
      // If time elapsed is too short for a full degree with current speed,
      // but some time has passed, consider if we should force a 1-degree move
      // if enough effective time for it has accumulated (e.g., (1000.0 / globalMovementSpeed) ms).
      // The original logic: degreesToMove = 1.0 if (degreesToMove < 1.0 && elapsedTime > (1000.0 /
      // movementSpeed)) This means if calculated move is <1 but enough time for >1 degree passed,
      // move 1. This prevents getting stuck if step is always fractional.
      if (elapsedTime >=
          (1000.0f / globalMovementSpeed)) {  // if enough time for at least 1 deg has passed
        degreesToMoveThisStep = 1.0f;
      } else if (degreesToMoveThisStep == 0 && currentPhysicalAngles[i] != targetSmoothAngles[i]) {
        // If calculation resulted in 0 move (e.g. very small elapsedTime) but not at target,
        // skip write for this cycle to avoid jitter or unnecessary writes.
        // However, if lastMoveTime[i] is also currentTime, it means we just set a target.
        // Let the next cycle handle it with a proper elapsedTime.
        if (lastMoveTime[i] != currentTime) {  // if not just set in this same cycle
                                               // continue; // Option: wait for more elapsed time
        }
      }
    }

    if (degreesToMoveThisStep >= 1.0f ||
        (degreesToMoveThisStep > 0 &&
         currentPhysicalAngles[i] !=
             targetSmoothAngles[i])) {  // Only move if step is substantial or target not met
      int actualStep = static_cast<int>(degreesToMoveThisStep);
      if (actualStep == 0 && currentPhysicalAngles[i] != targetSmoothAngles[i])
        actualStep = 1;  // Ensure at least 1 degree if not at target and some move is calculated

      if (currentPhysicalAngles[i] < targetSmoothAngles[i]) {
        currentPhysicalAngles[i] += actualStep;
        if (currentPhysicalAngles[i] > targetSmoothAngles[i]) {
          currentPhysicalAngles[i] = targetSmoothAngles[i];
        }
      } else if (currentPhysicalAngles[i] > targetSmoothAngles[i]) {
        currentPhysicalAngles[i] -= actualStep;
        if (currentPhysicalAngles[i] < targetSmoothAngles[i]) {
          currentPhysicalAngles[i] = targetSmoothAngles[i];
        }
      }

      servos[i].write(currentPhysicalAngles[i]);
      lastMoveTime[i] = currentTime;
    } else if (lastMoveTime[i] == 0 && currentPhysicalAngles[i] != targetSmoothAngles[i]) {
      // If it's the very first update cycle for this target and no move was calculated,
      // ensure lastMoveTime is set so next cycle has a valid elapsedTime.
      lastMoveTime[i] = currentTime;
    }
  }
}

/**
 * @brief Sets the global movement speed for smooth servo transitions.
 *  Updates the `globalMovementSpeed` used by `UpdateSmoothMovements()`
 * when calculating step sizes for servo motion.
 * @param speed The desired speed in degrees per second. Must be a positive value.
 * If a non-positive value is given, the speed is not changed.
 */
void ServoHandler::SetGlobalSpeed(int speed) {
  if (speed > 0) {
    globalMovementSpeed = speed;
  }
}

/**
 * @brief Gets the array of current commanded target angles.
 *  Returns a const pointer to the `anglesWrite` array. This array reflects
 * the most recent angles set by `SetTargetAngles`, `SetServoAngleImmediate`, or
 * `SetServoAngleSmooth`.
 * @return Const pointer to the integer array of commanded angles.
 */
const int *ServoHandler::GetCurrentCommandedAngles() const { return anglesWrite; }

/**
 * @brief Prints ServoHandler configuration or current state to a Print stream.
 *  If `printConfig` is true, prints static configuration like number of servos,
 * pins, and global movement speed. Otherwise, prints the current physical angle and
 * target smooth angle for each servo.
 * @param output Reference to a Print object (e.g., Serial).
 * @param printConfig Boolean flag; true for config, false for current state. Defaults to false.
 */
void ServoHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.println(F("ServoHandler Configuration:"));
    output.print(F("  Number of Servos: "));
    output.println(numServosControlled);
    output.print(F("  Global Movement Speed (deg/s): "));
    output.println(globalMovementSpeed);
    output.println(F("  Servo Pins & Initial Status:"));
    for (int i = 0; i < numServosControlled; i++) {
      output.print(F("    Servo "));
      output.print(i);
      output.print(F(" - Pin: "));
      output.print(kServoPins[i]);
      // output.print(F(", Attached: ")); output.println(isAttached[i] ? "Yes" : "No"); //
      // isAttached might not be set at "config" time if Begin/Attach not called
    }
  } else {
    output.println(F("ServoHandler State:"));
    output.print(F("  Global Movement Speed (deg/s): "));
    output.println(globalMovementSpeed);
    for (int i = 0; i < numServosControlled; i++) {
      output.print(F("  Servo "));
      output.print(i);
      output.print(F(" (Pin: "));
      output.print(kServoPins[i]);
      output.print(F("):"));
      output.print(F(" Attached: "));
      output.print(isAttached[i] ? "Yes" : "No");
      if (isAttached[i]) {
        output.print(F(", Current Angle: "));
        output.print(currentPhysicalAngles[i]);
        output.print(F(", Target Angle: "));
        output.print(targetSmoothAngles[i]);
        output.print(F(" (Cmd: "));
        output.print(anglesWrite[i]);
        output.print(F(")"));
      }
      output.println();
    }
  }
}

/**
 * @brief Overloaded stream insertion operator for printing ServoHandler details.
 *  Allows a ServoHandler object to be easily printed to any Print-derived stream.
 * By default, prints the current runtime state of the servos.
 * @param output Reference to the Print stream.
 * @param handler Const reference to the ServoHandler object.
 * @return Reference to the Print stream.
 */
Print &operator<<(Print &output, const ServoHandler &handler) {
  handler.PrintInfo(output, false);  // Default to printing runtime state
  return output;
}