#ifndef PathHandler_H
#define PathHandler_H

#include <Arduino.h>
#include <vector>
#include "../drive/math/Pose2D.h"
#include "../drive/VectorRobotDrivePID.h"

// Define the tolerance for reaching a waypoint
#define INTOLERANCEREACHED 0.1f // Adjustable as needed

class PathHandler
{
public:
    PathHandler(VectorRobotDrivePID &robotDrive);

    void addWaypoint(const Pose2D &pose); // Add a waypoint to the path
    void clearPath();                     // Clear all waypoints
    bool executePath();                   // Execute the path step by step
    void skipToNextPath();                // Skip to the next waypoint
    void addWaypoints(const std::vector<Pose2D> &waypoints);

private:
    VectorRobotDrivePID &drive; // Reference to the robot drive
    std::vector<Pose2D> path;   // List of waypoints
    size_t currentPathIndex;    // Index of the current waypoint

    bool hasReachedWaypoint(const Pose2D &target); // Check if the robot reached the target
};

#endif
