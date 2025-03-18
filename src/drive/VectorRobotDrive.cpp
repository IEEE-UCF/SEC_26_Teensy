#include "VectorRobotDrive.h"

VectorRobotDrive::VectorRobotDrive(const MotorSetup motorSetups[], int numMotors, Print &output)
    : SimpleRobotDrive(motorSetups, numMotors, output),
      speedPose(0, 0, 0),
      prevSpeedPose(0, 0, 0) {}

/**
 * Set motor speeds based on speedPose. X and Y are in In/S, theta is in Rad/S
 * @param speedPose speeds to set
 */
void VectorRobotDrive::Set(const Pose2D &speedPose)
{
    static elapsedMicros timer = 0;
    if (timer <= 500)
        return;
    timer = 0;
    // Compile-time constant coefficients [X, Y, Theta]
    static constexpr float motorCoeffs[3][3] = {
        {0.0f, 1.0f, -1.0f}, // Left motor
        {1.5f, 0.0f, 0.0f},  // Back motor
        {0.0f, 1.0f, 1.0f}   // Right motor
    };

    /*// Validate TRACK_WIDTH before calculations
    if (TRACK_WIDTH < 1e-6f)
    {
        output.println(F("Error: Invalid TRACK_WIDTH!"));
        return;
    }*/
    accelerationConstraint(speedPose);

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
        const float xTerm = constrain(motorCoeffs[i][0] * speedPose.getX(), -MAX_VELOCITY, MAX_VELOCITY);
        const float yTerm = constrain(motorCoeffs[i][1] * speedPose.getY(), -MAX_VELOCITY, MAX_VELOCITY);
        const float thetaTerm = constrain(motorCoeffs[i][2] * speedPose.getTheta(), -MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY) * TRACK_WIDTH * 0.5;

        const float motorSpeed = (xTerm + yTerm + thetaTerm) / WHEEL_CIRCUMFERENCE / MOTOR_RPS_NOLOAD * 255.0f;

        motors[i]->Set(static_cast<int>(constrain(motorSpeed, -255, 255)));
    }
}

void VectorRobotDrive::accelerationConstraint(Pose2D newPose)
{
    static elapsedMicros dt = 0;
    // Calculate time delta for consistent acceleration limiting
    float deltaTime = dt / 1000000.0f; // Convert to seconds

    // Constrain the x, y, and theta components of speedPose
    float deltaX = constrain(newPose.getX() - speedPose.getX(),
                             -MAX_ACCELERATION * deltaTime,
                             MAX_ACCELERATION * deltaTime);

    float deltaY = constrain(newPose.getY() - speedPose.getY(),
                             -MAX_ACCELERATION * deltaTime,
                             MAX_ACCELERATION * deltaTime);

    float deltaTheta = constrain(newPose.getTheta() - speedPose.getTheta(),
                                 -MAX_ANGULAR_ACCELERATION * TRACK_WIDTH * 0.5 * deltaTime,
                                 MAX_ANGULAR_ACCELERATION * TRACK_WIDTH * 0.5 * deltaTime);

    // Update speedPose with constrained values
    Pose2D deltaPose(deltaX, deltaY, deltaTheta);
    speedPose.add(deltaPose);
    dt = 0;
}

/**
 * Sets motor speeds based on joystick input. -1 to 1. Pass yaw in
 * @param x x
 * @param y y
 * @param theta theta
 * @param yaw yaw of the robot
 */
Pose2D VectorRobotDrive::CalculateRCVector(float x, float y, float theta, float yaw)
{
    Pose2D speedPose(x, y, theta);
    Pose2D scalePose(MAX_VELOCITY, MAX_VELOCITY, -MAX_ANGULAR_VELOCITY);
    speedPose.multElement(scalePose).normalize(); // scales to real-world velocity, then normalizes
    float xymag = speedPose.getXyMag();
    speedPose.setXyMag(constrain(xymag, -MAX_VELOCITY, MAX_VELOCITY));
    speedPose.unnormalize(); // unnormalize constrained xymag

    Pose2D angleOffsetPose = Pose2D(0, 0, yaw).fixTheta();
    return speedPose.rotateVector(angleOffsetPose.getTheta()); // rotate vector by opposite current orientation
}

Pose2D VectorRobotDrive::GetVelocity() const
{
    return speedPose;
}