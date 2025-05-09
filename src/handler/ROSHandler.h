#ifndef ROSHANDLER_H
#define ROSHANDLER_H

// enables USB communication for ROS serial
#define USE_USBCON

// ROS and TF dependencies
#include <elapsedMillis.h>             // Utility for timing without blocking
#include <ros.h>                       // ROS communication handler
#include <ros/time.h>                  // ROS time utilities
#include <tf/transform_broadcaster.h>  // For broadcasting transform frames

#include "../drive/math/Pose2D.h"  // Custom Pose2D struct for robot position

// class to handle ROS communication and transformations
class ROSHandler {
 public:
  ROSHandler();                     // constructor
  void Setup();                     // initalizes ROS node
  void Update(const Pose2D &pose);  // publishes transform updates based on robot pose

 private:
  ros::NodeHandle nh;                    // ROS node handle for communication
  geometry_msgs::TransformStamped t;     // messages type for transformation
  tf::TransformBroadcaster broadcaster;  // broadcasts transform to ROS

  unsigned long minDelay = 10;  // minimum update delay (ms) to prevent spam

  // frame names used in ROS transform tree
  const char *base_link = "/base_link";  // robot base frame
  const char *odom = "/odom";            // odometry frame
};

#endif