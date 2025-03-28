#include "PathHandler.h"

PathHandler::PathHandler(VectorRobotDrivePID &robotDrive)
    : drive(robotDrive), currentPathIndex(0) {}

void PathHandler::addWaypoint(const Pose2D &pose)
{
    path.push_back(pose);
}

void PathHandler::clearPath()
{
    path.clear();
    currentPathIndex = 0;
}

bool PathHandler::executePath()
{
    if (currentPathIndex >= path.size())
    {
        return true; // Path execution complete
    }

    const Pose2D &target = path[currentPathIndex];
    drive.SetTarget(target);

    // Check if the robot has reached the current waypoint
    if (hasReachedWaypoint(target))
    {
        currentPathIndex++;
    }

    return false; // Path execution still in progress
}

void PathHandler::skipToNextPath()
{
    if (currentPathIndex < path.size())
    {
        currentPathIndex++; // Move to the next waypoint
    }
}

bool PathHandler::hasReachedWaypoint(const Pose2D &target)
{
    Pose2D currentPose = drive.GetPosition();
    Pose2D targetPose = target;
    Pose2D delta = targetPose.subtract(currentPose);
    float deltaxy = delta.normalize().getXyMag();
    return (deltaxy <= INTOLERANCEREACHED);
}
