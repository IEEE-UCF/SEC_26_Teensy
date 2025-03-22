#include "VectorRobotDrive.h"

VectorRobotDrive::VectorRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output)
    : SimpleRobotDrive(motorSetups, numMotors, output),
      currentSpeedPose(0, 0, 0) {}

/**
 * Set motor speeds based on speedPose. X and Y are in In/S, theta is in Rad/S.
 * Expects to be velocity constrained already.
 * @param speedPose speeds to set
 */
void VectorRobotDrive::Set(const Pose2D &speedPose)
{
    currentSpeedPose.setX(speedPose.getX());
    currentSpeedPose.setY(speedPose.getY());
    currentSpeedPose.setThetaRadians(speedPose.getTheta());
    static elapsedMicros timer = 0;
    if (timer <= 1000)
        return;
    timer = 0;
    // Compile-time constant coefficients [X, Y, Theta]
    static constexpr float motorCoeffs[3][3] = {
        {1.0f, 0.0f, -1.0f}, // Left motor
        {0.0f, 1.15f, 0.0f}, // Back motor
        {1.0f, 0.0f, 1.0f}   // Right motor
    };

    /*// Validate TRACK_WIDTH before calculations
    if (TRACK_WIDTH < 1e-6f)
    {
        output.println(F("Error: Invalid TRACK_WIDTH!"));
        return;
    }*/

    // Write speeds to motors
    for (size_t i = 0; i < static_cast<size_t>(numMotors); ++i)
    {
        if (i >= motors.size())
        {
            output.println(F("Error: Motor index out of bounds"));
            continue;
        }

        // input MAX_VELOCITY for each motor isn't really the correct way to constrain things here,
        // but the input magnitude should already be constrained to max_velocity anyways
        const float xTerm = constrain(motorCoeffs[i][0] * currentSpeedPose.getX(), -MAX_VELOCITY, MAX_VELOCITY);
        const float yTerm = constrain(motorCoeffs[i][1] * currentSpeedPose.getY(), -MAX_VELOCITY, MAX_VELOCITY);
        const float thetaTerm = constrain(motorCoeffs[i][2] * currentSpeedPose.getTheta(), -MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY) * TRACK_WIDTH * 0.5;

        const float motorSpeed = (xTerm + yTerm + thetaTerm) / WHEEL_CIRCUMFERENCE / MOTOR_RPS_NOLOAD * 255.0f;

        motors[i]->Set(static_cast<int>(constrain(motorSpeed, -255, 255)));
    }
}

Pose2D VectorRobotDrive::ConstrainNewSpeedPose(Pose2D newSpeedPose)
{
    static elapsedMicros dt = 0;
    if (dt <= 1000)
        return currentSpeedPose;
    // Calculate time delta for consistent acceleration limiting
    float deltaTime = dt / 1000000.0f; // Convert to seconds

    // Constrain the x, y, and theta components of currentSpeedPose
    float deltaX = constrain(newSpeedPose.getX() - currentSpeedPose.getX(),
                             -MAX_ACCELERATION * deltaTime,
                             MAX_ACCELERATION * deltaTime);

    float deltaY = constrain(newSpeedPose.getY() - currentSpeedPose.getY(),
                             -MAX_ACCELERATION * deltaTime,
                             MAX_ACCELERATION * deltaTime);

    float deltaTheta = constrain(newSpeedPose.getTheta() - currentSpeedPose.getTheta(),
                                 -MAX_ANGULAR_ACCELERATION * TRACK_WIDTH * 0.5 * deltaTime,
                                 MAX_ANGULAR_ACCELERATION * TRACK_WIDTH * 0.5 * deltaTime);

    Pose2D deacceleratingScalar(isDeaccelerating(newSpeedPose.getX(), currentSpeedPose.getX()) ? 2 : 1,
                                isDeaccelerating(newSpeedPose.getY(), currentSpeedPose.getY()) ? 2 : 1,
                                isDeaccelerating(newSpeedPose.getTheta(), currentSpeedPose.getTheta()) ? 2 : 1);

    Pose2D deltaPose(deltaX, deltaY, deltaTheta);
    deltaPose.multElement(deacceleratingScalar);
    // Update currentSpeedPose with constrained values
    // currentSpeedPose.add(deltaPose);
    Pose2D tempPose(currentSpeedPose.getX(), currentSpeedPose.getY(), currentSpeedPose.getTheta());
    tempPose.add(deltaPose).constrainXyMag(MAX_VELOCITY).constrainTheta(MAX_ANGULAR_VELOCITY);
    dt = 0;
    return tempPose;
}

bool VectorRobotDrive::isDeaccelerating(float newValue, float oldValue)
{
    return (abs(newValue) - abs(oldValue)) < 0;
}

/**
 * Sets motor speeds based on joystick input. -1 to 1. Pass yaw in
 * @param x x
 * @param y y
 * @param theta theta
 * @param yaw yaw of the robot
 */
Pose2D VectorRobotDrive::CalculateRCVector(float x, float y, float theta, float yaw, bool positionControl)
{
    Pose2D speedPose((abs(x) > 0.05f) ? x : 0.0f,
                     (abs(y) > 0.05f) ? y : 0.0f,
                     (abs(theta) > 0.05f) ? theta : 0.0f);
    Pose2D scalePose(MAX_VELOCITY, MAX_VELOCITY, -MAX_ANGULAR_VELOCITY);
    speedPose.multElement(scalePose).constrainXyMag(MAX_VELOCITY).constrainTheta(MAX_ANGULAR_VELOCITY);
    yaw = (positionControl) ? 0 : yaw;
    Pose2D angleOffsetPose = Pose2D(0, 0, -yaw + DRIVER_START_OFFSET).fixTheta();
    return speedPose.rotateVector(angleOffsetPose.getTheta()); // rotate vector by opposite current orientation
}