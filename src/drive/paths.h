#ifndef PATHS_H
#define PATHS_H
#include "math/Pose2D.h"
#include "LocalizationEncoder.h"

namespace Paths
{
    std::vector<Pose2D> test_1 = {
        Pose2D(0, 30, 0.5 * PI), // Wayp 1
        Pose2D(30, 30, 0.5 * PI) // Wayp 2
    };

    std::vector<Pose2D> straight_back_40 = {
        Pose2D(0, 40, 0.5 * PI),
        Pose2D(0, 0, 0.5 * PI) // Final Wayp
    };

    std::vector<Pose2D> side_left_40 = {
        Pose2D(-40, 0, 0.5 * PI),
        Pose2D(0, 0, 0.5 * PI) // Final Wayp
    };

    std::vector<Pose2D> side_left_rotate_40 = {
        Pose2D(-40, 0, -0.5 * PI),
        Pose2D(0, 0, 0.5 * PI) // Final Wayp
    };

    std::vector<Pose2D> andrew_waypoint = {
        Pose2D(STARTX, STARTY, 0.5 * PI),
        Pose2D(8, 27.5, 0.5 * PI),
        Pose2D(27.5, 41, 0.5 * PI),
        Pose2D(51, 3, 0.5 * PI),
        Pose2D(40, 22.5, 0.5 * PI),
        Pose2D(70, 22.5, 0.5 * PI),
        Pose2D(40, 22.5, 0.5 * PI),
    };

    std::vector<Pose2D> slam_left_corner = {
        Pose2D(3, 10, 0.5 * PI),
        Pose2D(3, 3, 0.5 * PI),
    };

    std::vector<Pose2D> beacon_position = {
        Pose2D(8, 27.5, 0.5 * PI),
    };

    std::vector<Pose2D> beacon_pullout = {
        Pose2D(20, 27.5, 0.5 * PI),
    };
}
#endif