#include "ROSHandler.h" // include the ROSHandler header file

// constructor (empty because no special setup is needed on object creation)
ROSHandler::ROSHandler() {}

// initializes the ROS node and TF broadcaster
void ROSHandler::Setup()
{
    nh.initNode();        // start ROS node communication
    broadcaster.init(nh); // initialize the transform broadcaster
}

// updates and publishes the transform based on the robot's pose
void ROSHandler::Update(const Pose2D &pose)
{
    static elapsedMillis timer = 0; // timer to control update frequency
    if (timer > minDelay)           // check if enough time has passed since the last update
    {
        timer = 0; // reset timer after sending update

        // set transformation frame names
        t.header.frame_id = odom;     // parent frame (odometry reference)
        t.child_frame_id = base_link; // child frame (robot base)

        // set translation (robot's position in the odom frame)
        t.transform.translation.x = pose.getX();
        t.transform.translation.y = pose.getY();
        t.transform.translation.z = 0; // sssume movement is in 2D (z = 0)

        // corrected quaternion representation for rotation
        t.transform.rotation.x = 0.0;                        // no rotation around x-axis
        t.transform.rotation.y = 0.0;                        // no rotation around y-axis
        t.transform.rotation.z = sin(pose.getTheta() / 2.0); // convert yaw angle to quaternion
        t.transform.rotation.w = cos(pose.getTheta() / 2.0); // quaternion normalization

        // timestamp for the transformation
        t.header.stamp = nh.now();

        // broadcast the transformation to ROS
        broadcaster.sendTransform(t);

        // handle ROS callbacks (ensures communication stays alive)
        nh.spinOnce();
    }
}
