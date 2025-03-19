#include "PIDDriveController.h"

PIDDriveController::PIDDriveController(const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig)
    : xPID(xConfig),
      yPID(yConfig),
      thetaPID(thetaConfig) {}

Pose2D PIDDriveController::Step(const Pose2D &currentPose, const Pose2D &targetPose)
{
    float xSpeed = xPID.Step(currentPose.getX(), targetPose.getY());
    float ySpeed = yPID.Step(currentPose.getY(), targetPose.getY());
    float thetaSpeed = thetaPID.Step(currentPose.getTheta(), targetPose.getTheta());
    Pose2D speedPose = Pose2D(xSpeed, ySpeed, thetaSpeed).constrainXyMag(MAX_VELOCITY).constrainTheta(MAX_ANGULAR_VELOCITY);
    return speedPose;
}

/*PIDDrive::PIDDrive(const PIDConfig &xConfig, const PIDConfig &yConfig, const PIDConfig &thetaConfig, Print &serialOutput)
    : xPID(xConfig), yPID(yConfig), thetaPID(thetaConfig), serialOutput(serialOutput) {}


Pose2D PIDDrive::Step(const Pose2D &currentPose, const Pose2D &targetPose)
{
    double xCommand = xPID.Step(currentPose.x, targetPose.x);
    double yCommand = yPID.Step(currentPose.y, targetPose.y);
    double thetaCommand = thetaPID.Step(currentPose.theta, targetPose.theta);
    static elapsedMillis timer = 0;
    static Pose2D previousCommand(0, 0, 0);
    if (timer > 10)
    {
        Pose2D command(xCommand, yCommand, thetaCommand);
        // Velocity constraints
        if (command.magnitude() > MAX_VELOCITY)
        {
            command.normalize(serialOutput);
            command.xymag = MAX_VELOCITY;
            command.unnormalize(serialOutput);
        }
        command.theta = constrain(command.theta, -MAX_ANGULAR_VELOCITY, MAX_ANGULAR_VELOCITY);

        // Acceleration constraints
        Pose2D change = command;
        change.subtract(previousCommand);
        float dv = change.magnitude() / (timer / 1000);
        float adv = change.theta;
        if (dv > MAX_ACCELERATION)
        {
            command.normalize(serialOutput);
            command.xymag = previousCommand.magnitude() + MAX_ACCELERATION;
            command.unnormalize(serialOutput);
        }
        if (abs(adv) > MAX_ANGULAR_ACCELERATION)
        {
            command.theta = previousCommand.theta + (abs(adv) / adv) * MAX_ANGULAR_ACCELERATION;
        }
        previousCommand = command;
    }
    return previousCommand;
}*/