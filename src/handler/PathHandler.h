/**
 * @file PathHandler.h
 * @author Aldem Pido
 * @brief Defines the PathHandler class for managing and executing waypoints for robot navigation.
 * @date May 19, 2025
 * @defgroup navigation Path Planning System
 * This group contains components for managing and executing waypoints efficiently.
 */

#ifndef PATHHANDLER_H
#define PATHHANDLER_H

#include <Arduino.h>

#include <vector>

#include "../drive/VectorRobotDrivePID.h"  // Assuming this path is correct relative to PathHandler.h
#include "../drive/math/Pose2D.h"  // Assuming this path is correct relative to PathHandler.h

// Define constants for waypoint tracking
#define INTOLERANCEREACHED \
  2.5f  ///< Distance tolerance in units (e.g., cm) for considering a waypoint reached.
#define INRADIANSREACHED \
  0.1f  ///< Orientation tolerance in radians for considering a waypoint reached.
#define MINTIMEPAUSE \
  0.5f  ///< Minimum pause time in seconds at a waypoint before moving to the next.
#define GLOBAL_TIMEOUT 5.0f  ///< Default timeout in seconds for reaching each waypoint.

/**
 * @class PathHandler
 * @ingroup navigation
 * @brief Manages a sequence of waypoints for robot navigation.
 *  This class allows for adding waypoints, executing a path defined by these waypoints,
 * handling timeouts, and checking if waypoints have been reached. It interacts with a
 * VectorRobotDrivePID object to control the robot's movement and to get its current pose.
 */
class PathHandler {
 public:
  PathHandler(VectorRobotDrivePID &robotDrive);
  void addWaypoint(const Pose2D &pose);
  void addWaypoints(const std::vector<Pose2D> &waypoints);
  void clearPath();
  bool executePath();
  void skipToNextPath();
  void setTimeout(float seconds);

 private:
  VectorRobotDrivePID &drive;       ///< Reference to the robot's drive system.
  std::vector<Pose2D> path;         ///< Stores the sequence of waypoints.
  size_t currentPathIndex;          ///< Index of the current target waypoint in the `path` vector.
  unsigned long lastWaypointTime;   ///< Timestamp (milliseconds) when the last waypoint was
                                    ///< considered reached, used for pausing.
  unsigned long waypointStartTime;  ///< Timestamp (milliseconds) when the robot started moving
                                    ///< towards the current waypoint.
  float timeoutSeconds;             ///< Timeout duration in seconds for reaching a waypoint.

  bool hasReachedWaypoint(const Pose2D &target);
  bool hasTimedOut();
};

#endif  // PATHHANDLER_H