#include "ROSHandler.h"

ROSHandler::ROSHandler() {}

/**
 * Begin ros serial
 */
void ROSHandler::Setup()
{
    nh.initNode();
    broadcaster.init(nh);
}

/**
 * Update ros serial.
 *
 * @param pose Transform of robot
 */
void ROSHandler::Update(const Pose2D &pose)
{
    static elapsedMillis timer = 0;
    if (timer > minDelay)
    {
        timer = 0;
        t.header.frame_id = odom;
        t.child_frame_id = base_link;
        t.transform.translation.x = pose.x;
        t.transform.translation.y = pose.y;
        t.transform.translation.z = 0;
        t.transform.rotation.x = 0.0;
        t.transform.rotation.y = 0.0;
        t.transform.rotation.z = pose.theta;
        t.transform.rotation.w = 1.0;
        t.header.stamp = nh.now();
        broadcaster.sendTransform(t);
        nh.spinOnce();
    }
}