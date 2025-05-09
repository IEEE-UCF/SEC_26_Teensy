/*
Aldem Pido - 4/1/25
PathHandler.h - handles waypoints and target positions
*/
#ifndef PathHandler_H
#define PathHandler_H

#include <Arduino.h>

#include <vector>

#include "../drive/VectorRobotDrivePID.h"
#include "../drive/math/Pose2D.h"

// Define the tolerance for reaching a waypoint
#define INTOLERANCEREACHED 2.5f  // Adjustable as needed
#define INRADIANSREACHED 0.1f
#define MINTIMEPAUSE 0.5f
#define GLOBAL_TIMEOUT 5.0f  // Timeout in seconds for each waypoint

class PathHandler {
 public:
  PathHandler(VectorRobotDrivePID &robotDrive);

  void addWaypoint(const Pose2D &pose);  // Add a waypoint to the path
  void clearPath();                      // Clear all waypoints
  bool executePath();                    // Execute the path step by step
  void skipToNextPath();                 // Skip to the next waypoint
  void addWaypoints(const std::vector<Pose2D> &waypoints);
  void setTimeout(float seconds);  // Set a custom timeout value

 private:
  VectorRobotDrivePID &drive;  // Reference to the robot drive
  std::vector<Pose2D> path;    // List of waypoints
  size_t currentPathIndex;     // Index of the current waypoint
  long lastWaypointTime;       // Time when reached waypoint for pause
  long waypointStartTime;      // Time when started approaching current waypoint
  float timeoutSeconds;        // Timeout value in seconds
  bool hasReachedWaypoint(
      const Pose2D &target);  // Check if the robot reached the target
  bool hasTimedOut();         // Check if current waypoint has timed out
};

#endif