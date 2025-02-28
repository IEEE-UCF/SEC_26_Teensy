#ifndef ROSHANDLER_H
#define ROSHANDLER_H

#define USE_USBCON
#include <ros.h>
#include <ros/time.h>
#include <tf/transform_broadcaster.h>
#include <elapsedMillis.h>
#include "../drive/math/Pose2D.h"
class ROSHandler
{
public:
    ROSHandler();
    void Setup();
    void Update(const Pose2D &pose);

private:
    ros::NodeHandle nh;
    geometry_msgs::TransformStamped t;
    tf::TransformBroadcaster broadcaster;
    unsigned long minDelay = 10; // ms
    char base_link[11] = "/base_link";
    char odom[6] = "/odom";
};

#endif