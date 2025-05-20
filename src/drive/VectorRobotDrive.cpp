#include "VectorRobotDrive.h"

/**
 * @brief Constructs a VectorRobotDrive object.
 * @param motorSetups Array of motor configurations.
 * @param numMotors Number of motors.
 * @param output Output stream for logging.
 */
VectorRobotDrive::VectorRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output)
    : SimpleRobotDrive(motorSetups, numMotors, output),
      currentSpeedPose(0, 0, 0),
      idealSpeedPose(0, 0, 0) {}

/**
 * @brief Sets motor speeds based on velocity.
 * @param speedPose Velocity pose (X/Y in inches/sec, Theta in radians/sec).
 */
void VectorRobotDrive::Set(const Pose2D &speedPose) {
  currentSpeedPose = speedPose;

  static constexpr float motorCoeffs[3][3] = {
      {1.0f, 0.0f, -1.0f},  // Left motor
      {0.0f, 1.15f, 0.0f},  // Back motor
      {1.0f, 0.0f, 1.0f}    // Right motor
  };

  for (size_t i = 0; i < static_cast<size_t>(numMotors); ++i) {
    if (i >= motors.size()) {
      output.println(F("Error: Motor index out of bounds"));
      continue;
    }

    const float xTerm =
        constrain(motorCoeffs[i][0] * currentSpeedPose.getX(), -MAX_VELOCITY, MAX_VELOCITY);
    const float yTerm =
        constrain(motorCoeffs[i][1] * currentSpeedPose.getY(), -MAX_VELOCITY, MAX_VELOCITY);
    const float thetaTerm = constrain(motorCoeffs[i][2] * currentSpeedPose.getTheta(),
                                      -MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY) *
                            TRACK_WIDTH * 0.5;

    const float motorSpeed =
        (xTerm + yTerm + thetaTerm) / WHEEL_CIRCUMFERENCE / MOTOR_RPS_NOLOAD * 255.0f;

    motors[i]->Set(static_cast<int>(constrain(motorSpeed, -255, 255)));
  }
}

/**
 * @brief Constrains speed pose while considering acceleration limits.
 * @param newSpeedPose New velocity pose.
 * @return Constrained speed pose.
 */
Pose2D VectorRobotDrive::ConstrainNewSpeedPose(Pose2D newSpeedPose) {
  static elapsedMicros dt = 0;
  if (dt <= 1000) return currentSpeedPose;

  float deltaTime = dt / 1000000.0f;

  float deltaX = constrain(newSpeedPose.getX() - idealSpeedPose.getX(),
                           -MAX_ACCELERATION * deltaTime, MAX_ACCELERATION * deltaTime);
  float deltaY = constrain(newSpeedPose.getY() - idealSpeedPose.getY(),
                           -MAX_ACCELERATION * deltaTime, MAX_ACCELERATION * deltaTime);
  float deltaTheta = constrain(newSpeedPose.getTheta() - idealSpeedPose.getTheta(),
                               -MAX_ANGULAR_ACCELERATION * TRACK_WIDTH * 0.5 * deltaTime,
                               MAX_ANGULAR_ACCELERATION * TRACK_WIDTH * 0.5 * deltaTime);

  Pose2D deacceleratingScalar(
      isDeaccelerating(newSpeedPose.getX(), idealSpeedPose.getX()) ? 2 : 1,
      isDeaccelerating(newSpeedPose.getY(), idealSpeedPose.getY()) ? 2 : 1,
      isDeaccelerating(newSpeedPose.getTheta(), idealSpeedPose.getTheta()) ? 2 : 1);

  Pose2D deltaPose(deltaX, deltaY, deltaTheta);
  deltaPose.multElement(deacceleratingScalar);

  idealSpeedPose.add(deltaPose).constrainXyMag(MAX_VELOCITY).constrainTheta(MAX_ANGULAR_VELOCITY);

  dt = 0;
  return idealSpeedPose;
}

/**
 * @brief Determines if the robot is deaccelerating.
 * @param newValue New speed.
 * @param oldValue Previous speed.
 * @return True if deaccelerating, false otherwise.
 */
bool VectorRobotDrive::isDeaccelerating(float newValue, float oldValue) {
  return (abs(newValue) - abs(oldValue)) < 0;
}

/**
 * @brief Computes velocity vector from joystick inputs.
 * @param x X input (-1 to 1).
 * @param y Y input (-1 to 1).
 * @param theta Rotational input (-1 to 1).
 * @param yaw Current robot yaw.
 * @param positionControl If true, ignores yaw in computations.
 * @return Computed velocity vector.
 */
Pose2D VectorRobotDrive::CalculateRCVector(float x, float y, float theta, float yaw,
                                           bool positionControl) {
  Pose2D speedPose((abs(x) > 0.05f) ? x : 0.0f, (abs(y) > 0.05f) ? y : 0.0f,
                   (abs(theta) > 0.05f) ? theta : 0.0f);

  Pose2D scalePose(MAX_VELOCITY, MAX_VELOCITY, -MAX_ANGULAR_VELOCITY);
  speedPose.multElement(scalePose)
      .constrainXyMag(MAX_VELOCITY)
      .constrainTheta(MAX_ANGULAR_VELOCITY);

  yaw = positionControl ? 0 : yaw;
  Pose2D angleOffsetPose = Pose2D(0, 0, -yaw + DRIVER_START_OFFSET).fixTheta();

  return speedPose.rotateVector(angleOffsetPose.getTheta());
}
