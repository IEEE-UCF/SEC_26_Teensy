#include "LocalizationEncoder.h"

#include <Arduino.h>

#include "MOTORCONFIG.h"

/**
 * @brief Constructs a LocalizationEncoder object.
 */
LocalizationEncoder::LocalizationEncoder() : transform(0, 0, 0) {
  setPosition(Pose2D(STARTX, STARTY, 0));
}

/**
 * @brief Updates the robot's position based on encoder counts and yaw angle.
 * @param encoderCounts Array containing encoder counts {left, back, right}.
 * @param yaw Gyro yaw reading (in radians).
 */
void LocalizationEncoder::updatePosition(const long encoderCounts[3], float yaw) {
  const long leftChangeTicks = encoderCounts[0] - previousLeftTicks;
  const long backChangeTicks = encoderCounts[1] - previousBackTicks;
  const long rightChangeTicks = encoderCounts[2] - previousRightTicks;
  const float yawChange = yaw - previousYaw;

  previousLeftTicks = encoderCounts[0];
  previousBackTicks = encoderCounts[1];
  previousRightTicks = encoderCounts[2];
  previousYaw = yaw;

  const float leftDistance = leftChangeTicks * IN_PER_TICK;
  const float backDistance = backChangeTicks * IN_PER_TICK;
  const float rightDistance = rightChangeTicks * IN_PER_TICK;

  const float cosTheta = cosf(yaw);
  const float sinTheta = sinf(yaw);

  const float deltaX =
      ((leftDistance + rightDistance) * 0.5f * cosTheta) - (backDistance * sinTheta);

  const float deltaY =
      ((leftDistance + rightDistance) * 0.5f * sinTheta) + (backDistance * cosTheta);

  const float deltaTheta = yawChange;
  transform.add(Pose2D(deltaX, deltaY, deltaTheta)).fixTheta();
}

/**
 * @brief Returns the current position of the robot.
 * @return Pose2D containing the robot's position and orientation.
 */
Pose2D LocalizationEncoder::getPosition() const { return transform; }

/**
 * @brief Manually sets the robot's position.
 * @param transform New position and orientation.
 */
void LocalizationEncoder::setPosition(const Pose2D &transform) { this->transform = transform; }

/**
 * @brief Prints localization information.
 * @param output Output stream for logging.
 */
void LocalizationEncoder::PrintInfo(Print &output) const {
  output.println(F("Localization Encoder Information:"));
  output.print(F("Location "));
  output << transform;
}

/**
 * @brief Overloaded stream operator for printing encoder information.
 * @param output Output stream.
 * @param encoder LocalizationEncoder instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const LocalizationEncoder &encoder) {
  encoder.PrintInfo(output);
  return output;
}
