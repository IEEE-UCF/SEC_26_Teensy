#include "ServoHandler.h"

#include <Arduino.h>

ServoHandler::ServoHandler(int *kServo, int numServos)
    : kServo(kServo),
      numServos(numServos),
      movementSpeed(DEFAULT_MOVEMENT_SPEED) {}

void ServoHandler::Begin() {
  servos = new Servo[numServos];
  anglesWrite = new int[numServos];
  currentAngles = new int[numServos];
  targetAngles = new int[numServos];
  lastMoveTime = new unsigned long[numServos];
  attached = new bool[numServos];

  for (int i = 0; i < numServos; ++i) {
    anglesWrite[i] = 90;  // Default to center position
    currentAngles[i] = 90;
    targetAngles[i] = 90;
    lastMoveTime[i] = 0;
    attached[i] = false;
  }
}

void ServoHandler::Attach() {
  for (int i = 0; i < numServos; ++i) {
    if (!attached[i])  // Only attach servos if they are not attached
    {
      servos[i].attach(kServo[i]);
      attached[i] = true;
    }
  }
}

void ServoHandler::Detach() {
  for (int i = 0; i < numServos; ++i) {
    if (attached[i])  // Only detach servos if they are attached
    {
      servos[i].detach();
      attached[i] = false;
    }
  }
}

void ServoHandler::Set(int *anglesWrite) {
  for (int i = 0; i < numServos; i++) {
    targetAngles[i] = anglesWrite[i];
    this->anglesWrite[i] = anglesWrite[i];
  }
}

void ServoHandler::Write() {
  for (int i = 0; i < numServos; i++) {
    WriteServoAngle(i, anglesWrite[i]);
  }
}

void ServoHandler::WriteServoAngle(int index, int angle) {
  if (index >= 0 && index < numServos) {
    // Set both current and target angle for immediate movement
    servos[index].write(angle);
    currentAngles[index] = angle;
    targetAngles[index] = angle;
    anglesWrite[index] = angle;
  }
}

void ServoHandler::WriteServoAngleSmooth(int index, int angle, int speed) {
  if (index >= 0 && index < numServos) {
    // Set the target angle for smooth transition
    targetAngles[index] = constrain(angle, ANGLE_LOW, ANGLE_HIGH);
    anglesWrite[index] = targetAngles[index];

    // Use custom speed if provided, otherwise use the global speed
    if (speed > 0) {
      movementSpeed = speed;
    }
  }
}

void ServoHandler::Update() {
  unsigned long currentTime = millis();

  for (int i = 0; i < numServos; i++) {
    // Skip if the servo is already at the target position
    if (currentAngles[i] == targetAngles[i]) {
      continue;
    }

    // Calculate time elapsed since last move
    unsigned long elapsedTime = currentTime - lastMoveTime[i];

    // Calculate how many degrees we should move based on speed and elapsed time
    // movementSpeed is in degrees per second, so we convert to degrees per
    // millisecond
    float degreesToMove = movementSpeed * (elapsedTime / 1000.0);

    // Ensure we move at least 1 degree if enough time has passed
    if (degreesToMove < 1.0 && elapsedTime > (1000.0 / movementSpeed)) {
      degreesToMove = 1.0;
    }

    // Determine direction and amount to move
    if (currentAngles[i] < targetAngles[i]) {
      // Moving up
      currentAngles[i] +=
          min(degreesToMove, targetAngles[i] - currentAngles[i]);
    } else if (currentAngles[i] > targetAngles[i]) {
      // Moving down
      currentAngles[i] -=
          min(degreesToMove, currentAngles[i] - targetAngles[i]);
    }

    // Apply the new position
    servos[i].write(currentAngles[i]);

    // Update the time of this move
    lastMoveTime[i] = currentTime;
  }
}

void ServoHandler::SetSpeed(int speed) {
  if (speed > 0) {
    movementSpeed = speed;
  }
}

int *ServoHandler::Get() { return anglesWrite; }

void ServoHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("ServoHandler Configuration: "));
    output.print(F("Number of Servos: "));
    output.println(numServos);
    output.print(F("Movement Speed: "));
    output.print(movementSpeed);
    output.println(F(" degrees/second"));

    for (int i = 0; i < numServos; i++) {
      output.print(F("Servo "));
      output.print(i);
      output.print(F(" - Pin: "));
      output.println(kServo[i]);
    }
  } else {
    for (int i = 0; i < numServos; i++) {
      output.print(F("Servo "));
      output.print(i);
      output.print(F(" - Current Angle: "));
      output.print(currentAngles[i]);
      output.print(F(" - Target Angle: "));
      output.println(targetAngles[i]);
    }
  }
}

Print &operator<<(Print &output, const ServoHandler &handler) {
  handler.PrintInfo(output, false);
  return output;
}