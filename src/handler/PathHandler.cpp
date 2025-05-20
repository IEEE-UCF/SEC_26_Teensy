/**
 * @file PathHandler.cpp
 * @author Aldem Pido
 * @brief Implements the PathHandler class for managing robot navigation along a series of
 * waypoints.
 * @date May 19, 2025
 */
#include "PathHandler.h"

#include <Arduino.h>  // Required for millis() and Serial

/**
 * @brief Constructs a PathHandler object.
 *  Initializes the PathHandler with a reference to the robot's drive system.
 * Sets the current path index to 0, and initializes timing variables for waypoint
 * navigation and the default timeout.
 * @param robotDrive Reference to the robot's drive system (VectorRobotDrivePID).
 * This is used to control the robot's movement and to get its current position.
 */
PathHandler::PathHandler(VectorRobotDrivePID &robotDrive)
    : drive(robotDrive),
      currentPathIndex(0),
      lastWaypointTime(0),
      waypointStartTime(0),
      timeoutSeconds(GLOBAL_TIMEOUT) {}

/**
 * @brief Adds a single waypoint to the path.
 *  Appends the given Pose2D object to the end of the internal list of waypoints.
 * @param pose The Pose2D object representing the waypoint to be added. This includes x, y
 * coordinates and orientation (theta).
 */
void PathHandler::addWaypoint(const Pose2D &pose) { path.push_back(pose); }

/**
 * @brief Adds multiple waypoints to the path from a vector of Pose2D objects.
 *  Appends all Pose2D objects from the provided vector to the end of the
 * current path, maintaining their order.
 * @param waypoints A std::vector of Pose2D objects. These waypoints are appended to the
 * end of the current path in the order they appear in the vector.
 */
void PathHandler::addWaypoints(const std::vector<Pose2D> &waypoints) {
  path.insert(path.end(), waypoints.begin(), waypoints.end());
}

/**
 * @brief Clears all waypoints from the path.
 *  Removes all waypoints from the internal path list. It also resets the
 * current path index and timing variables (lastWaypointTime, waypointStartTime)
 * to their initial states, effectively stopping any ongoing navigation and preparing
 * for a new path.
 */
void PathHandler::clearPath() {
  path.clear();
  currentPathIndex = 0;
  lastWaypointTime = 0;
  waypointStartTime = 0;
}

/**
 * @brief Sets a custom timeout for reaching individual waypoints.
 *  Updates the time limit allowed for the robot to reach each waypoint.
 * The new timeout value will be used for subsequent waypoints.
 * @param seconds The timeout duration in seconds. Must be a positive value.
 * If a non-positive value is provided, the timeout remains unchanged.
 */
void PathHandler::setTimeout(float seconds) {
  if (seconds > 0) {
    timeoutSeconds = seconds;
  }
}

/**
 * @brief Executes the waypoint-based path navigation step by step.
 *  This function is intended to be called repeatedly (e.g., in a main loop).
 * It checks if the current path is empty. If not, it sets the current waypoint as the
 * target for the robot's drive system. It then checks if the robot has reached the
 * target waypoint or if the allocated time for the waypoint has timed out.
 * If a waypoint is reached, it initiates a pause (MINTIMEPAUSE) before advancing to
 * the next waypoint. If a timeout occurs, it logs a message and advances to the next
 * waypoint.
 * @return True if all waypoints in the path have been successfully reached (i.e., currentPathIndex
 * is beyond the end of the path). False if the path is still being executed or is empty.
 */
bool PathHandler::executePath() {
  if (currentPathIndex >= path.size()) {
    return true;  // All waypoints processed
  }

  const Pose2D &target = path[currentPathIndex];
  drive.SetTarget(target);  // Command the robot to move towards the target

  if (waypointStartTime == 0) {  // Initialize start time for the current waypoint
    waypointStartTime = millis();
  }

  if (hasReachedWaypoint(target)) {
    if (lastWaypointTime == 0) {  // Waypoint just reached, start pause timer
      lastWaypointTime = millis();
    } else if (millis() - lastWaypointTime >= MINTIMEPAUSE * 1000) {  // Pause finished
      currentPathIndex++;                                             // Move to the next waypoint
      lastWaypointTime = 0;                                           // Reset pause timer
      waypointStartTime = 0;  // Reset waypoint start timer for the next waypoint
    }
  } else if (hasTimedOut()) {
    // Handle waypoint timeout
    Serial.print("Waypoint ");
    Serial.print(currentPathIndex);
    Serial.println(" timed out. Moving to next waypoint.");

    currentPathIndex++;     // Move to the next waypoint
    lastWaypointTime = 0;   // Reset pause timer
    waypointStartTime = 0;  // Reset waypoint start timer
  } else {
    // Still moving towards the waypoint, ensure pause timer is reset if we were pausing
    lastWaypointTime = 0;
  }

  return false;  // Path execution still in progress
}

/**
 * @brief Skips the current target waypoint and moves to the next one in the path.
 *  If there are more waypoints in the path, this function increments the
 * currentPathIndex, effectively making the next waypoint the current target.
 * It also resets the timing variables (lastWaypointTime, waypointStartTime) for the
 * new current waypoint. If there are no more waypoints, this function has no effect.
 */
void PathHandler::skipToNextPath() {
  if (currentPathIndex < path.size()) {
    currentPathIndex++;
    lastWaypointTime = 0;
    waypointStartTime = 0;
  }
}

/**
 * @brief Determines if the robot has reached the specified target waypoint.
 *  Compares the robot's current pose (obtained from the drive system)
 * with the target waypoint's pose. The waypoint is considered reached if the
 * Euclidean distance in the XY plane is within INTOLERANCEREACHED and the
 * absolute difference in orientation (theta) is within INRADIANSREACHED.
 * Theta values are fixed to ensure correct comparison, e.g., by normalizing angles.
 * @param target The 2D pose (Pose2D) of the target waypoint.
 * @return True if the robot is within the defined tolerance (distance and orientation)
 * of the target waypoint, false otherwise.
 */
bool PathHandler::hasReachedWaypoint(const Pose2D &target) {
  Pose2D currentPose = drive.GetPosition();
  Pose2D delta =
      target.subtract(currentPose).fixTheta();   // Calculate difference and normalize theta
  float deltaxy = delta.normalize().getXyMag();  // Magnitude of XY difference
  float deltatheta = delta.getTheta();           // Difference in orientation (already fixed)
  // Ensure deltatheta is the shortest angle, e.g. by taking abs(fixTheta(deltatheta))
  // or ensuring fixTheta handles this appropriately. The provided snippet uses
  // delta.fixTheta().getTheta() which implies fixTheta normalizes to a range like -PI to PI.
  return ((deltaxy <= INTOLERANCEREACHED) && (abs(deltatheta) <= INRADIANSREACHED));
}

/**
 * @brief Checks if the time limit for reaching the current waypoint has been exceeded.
 *  This function compares the current time (millis()) with the time recorded
 * when the robot started moving towards the current waypoint (waypointStartTime).
 * If the elapsed time is greater than or equal to the configured timeoutSeconds,
 * the waypoint is considered timed out.
 * @return True if waypointStartTime is set (meaning we are navigating to a waypoint)
 * and the elapsed time since waypointStartTime exceeds timeoutSeconds. False otherwise.
 */
bool PathHandler::hasTimedOut() {
  return (waypointStartTime > 0) && (millis() - waypointStartTime >= timeoutSeconds * 1000);
}