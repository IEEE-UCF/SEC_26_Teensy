#include "PathHandler.h"
#include <Arduino.h>

PathHandler::PathHandler(VectorRobotDrivePID &robotDrive)
    : drive(robotDrive), currentPathIndex(0), lastWaypointTime(0),
      waypointStartTime(0), timeoutSeconds(GLOBAL_TIMEOUT) {}

void PathHandler::addWaypoint(const Pose2D &pose)
{
    path.push_back(pose);
}

void PathHandler::addWaypoints(const std::vector<Pose2D> &waypoints)
{
    for (const Pose2D &waypoint : waypoints)
    {
        path.push_back(waypoint);
    }
}

void PathHandler::clearPath()
{
    path.clear();
    currentPathIndex = 0;
    lastWaypointTime = 0;
    waypointStartTime = 0;
}

void PathHandler::setTimeout(float seconds)
{
    if (seconds > 0)
    {
        timeoutSeconds = seconds;
    }
}

bool PathHandler::executePath()
{
    if (currentPathIndex >= path.size())
    {
        return true; // Path execution complete
    }

    const Pose2D &target = path[currentPathIndex];
    drive.SetTarget(target);

    // Initialize the start time for the current waypoint if not set
    if (waypointStartTime == 0)
    {
        waypointStartTime = millis();
    }

    // Check if the robot has reached the current waypoint
    if (hasReachedWaypoint(target))
    {
        // If this is the first time reaching the waypoint, start the pause timer
        if (lastWaypointTime == 0)
        {
            lastWaypointTime = millis();
        }
        // Ensure the minimum pause time before moving to the next waypoint
        else if (millis() - lastWaypointTime >= MINTIMEPAUSE * 1000)
        {
            currentPathIndex++;
            lastWaypointTime = 0;  // Reset for the next waypoint
            waypointStartTime = 0; // Reset start time for the next waypoint
        }
    }
    // Check if the waypoint has timed out
    else if (hasTimedOut())
    {
        // Log timeout event
        Serial.print("Waypoint ");
        Serial.print(currentPathIndex);
        Serial.println(" timed out. Moving to next waypoint.");

        // Move to the next waypoint
        currentPathIndex++;
        lastWaypointTime = 0;
        waypointStartTime = 0; // Reset start time for the next waypoint
    }
    else
    {
        // Reset the pause timer if the waypoint hasn't been reached yet
        lastWaypointTime = 0;
    }

    return false; // Path execution still in progress
}

void PathHandler::skipToNextPath()
{
    if (currentPathIndex < path.size())
    {
        currentPathIndex++;
        lastWaypointTime = 0;
        waypointStartTime = 0; // Reset start time for the next waypoint
    }
}

bool PathHandler::hasReachedWaypoint(const Pose2D &target)
{
    Pose2D currentPose = drive.GetPosition();
    Pose2D targetPose = target;
    Pose2D delta = targetPose.subtract(currentPose).fixTheta();
    float deltaxy = delta.normalize().getXyMag();
    float deltatheta = delta.fixTheta().getTheta();
    return ((deltaxy <= INTOLERANCEREACHED) && (deltatheta <= INRADIANSREACHED));
}

bool PathHandler::hasTimedOut()
{
    // Check if the current waypoint has exceeded the timeout duration
    return (waypointStartTime > 0) &&
           (millis() - waypointStartTime >= timeoutSeconds * 1000);
}