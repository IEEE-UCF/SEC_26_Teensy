#ifndef PATHS_H
#define PATHS_H
#include "LocalizationEncoder.h"
#include "math/Pose2D.h"

#define BEACONX 9
#define BEACONY 28.5
#define NORTH (0.5 * PI)
#define EAST 0.0
#define SOUTH (-0.5 * PI)
#define WEST PI
#define GEODX 50.0
#define MAXY 47.0
#define NEBX 23.0
#define CENTERY (MAXY / 2.0)
#define MAXX 95.0
#define LEFTCAVEWALLX 55.0
#define MAINSWEEPLEFTX 0.0

namespace HardBox {
std::vector<Pose2D> startToSlamSW90 = {
    Pose2D(31.5, 6, NORTH),  // Beginning orientation
    Pose2D(31.5, BEACONY, NORTH), Pose2D(31.5, BEACONY, WEST),
    Pose2D(8, BEACONY, WEST),     Pose2D(8, BEACONY, NORTH),
    Pose2D(10, 2, NORTH),         Pose2D(2, 2, NORTH),
};

// Set to (6, 6, NORTH)

std::vector<Pose2D> supposedBeacon = {
    Pose2D(BEACONX, BEACONY - 2, NORTH),
};

// Move beacon arm down

std::vector<Pose2D> jostleBeacon = {
    Pose2D(BEACONX, BEACONY - 1, NORTH),
    Pose2D(BEACONX, BEACONY, NORTH),
    Pose2D(BEACONX, BEACONY + 2, NORTH),
    Pose2D(BEACONX - 1.5, BEACONY - 1, NORTH),
    Pose2D(BEACONX - 1.5, BEACONY, NORTH),
    Pose2D(BEACONX - 1.5, BEACONY + 2, NORTH),
    Pose2D(BEACONX, BEACONY, NORTH),

};

// Open geod servo

std::vector<Pose2D> positionGeoCSC = {
    Pose2D(BEACONX + 10, 3, WEST),  // slam into wall
    Pose2D(GEODX - 3, 3, WEST),     // align for clamping geodinium container
};

// Set to (43, 6, WEST)
// Close geod servo
std::vector<Pose2D> positionNebCSC_1 = {
    Pose2D(3, MAXY - 3, WEST),  // slam into top left corner
};

// Alt:
std::vector<Pose2D> depositCSC = {
    Pose2D(31.5, 20, WEST),
    Pose2D(31.5, 20, NORTH),
    Pose2D(10, 20, NORTH),
};

// Set to (6, MAXY-6, WEST)
std::vector<Pose2D> positionNebCSC_2 = {
    Pose2D(NEBX - 3, MAXY - 6, WEST),  // align for clamping neodinium container
};

// Close neod servo

std::vector<Pose2D> enterCave = {
    Pose2D(NEBX - 3, CENTERY, WEST),  // prep for entering cave
    Pose2D(NEBX - 3, CENTERY, EAST),
    Pose2D(MAXX - 3, CENTERY, EAST),  // slam into east wall
};

// Set to (MAXX - 6, CENTERY, EASY)

std::vector<Pose2D> setupCaveSweepNorth = {
    Pose2D(MAXX - 20, CENTERY, EAST),
    Pose2D(MAXX - 20, CENTERY, NORTH),
};

std::vector<Pose2D> caveSweepNorth = {
    Pose2D(MAXX - 6 * 1, CENTERY, NORTH),
    Pose2D(MAXX - 6 * 1, MAXY - 6, NORTH),
    Pose2D(MAXX - 6 * 1, CENTERY, NORTH),
    Pose2D(MAXX - 6 * 2, CENTERY, NORTH),
    Pose2D(MAXX - 6 * 2, MAXY - 6, NORTH),
    Pose2D(MAXX - 6 * 2, CENTERY, NORTH),
    Pose2D(MAXX - 6 * 3, CENTERY, NORTH),
    Pose2D(MAXX - 6 * 3, MAXY - 6, NORTH),
    Pose2D(MAXX - 6 * 3, CENTERY, NORTH),
    Pose2D(45, CENTERY, NORTH),
    Pose2D(45, CENTERY, EAST),
};

std::vector<Pose2D> setupCaveSweepSouth = {
    Pose2D(MAXX - 20, CENTERY, NORTH),
    Pose2D(MAXX - 20, CENTERY, SOUTH),
};

std::vector<Pose2D> caveSweepSouth = {
    Pose2D(MAXX - 6 * 1, CENTERY, SOUTH),
    Pose2D(MAXX - 6 * 1, 6, SOUTH),
    Pose2D(MAXX - 6 * 1, CENTERY, SOUTH),
    Pose2D(MAXX - 6 * 2, CENTERY, SOUTH),
    Pose2D(MAXX - 6 * 2, 6, SOUTH),
    Pose2D(MAXX - 6 * 2, CENTERY, SOUTH),
    Pose2D(MAXX - 6 * 3, CENTERY, SOUTH),
    Pose2D(MAXX - 6 * 3, 6, SOUTH),
    Pose2D(MAXX - 6 * 3, CENTERY, SOUTH),
    Pose2D(45, CENTERY, SOUTH),
    Pose2D(45, CENTERY, EAST),
};

std::vector<Pose2D> caveSweepReturn = {
    Pose2D(20, CENTERY, EAST),
    Pose2D(20, MAXY - 3, EAST),
    Pose2D(10, MAXY - 3, EAST),
};
// set normal starting position

// set (65, CENTERY, SOUTH)
std::vector<Pose2D> caveExit = {
    Pose2D(MAXX - 20, CENTERY, SOUTH),
    Pose2D(MAXX - 20, CENTERY, WEST),
    Pose2D(25, CENTERY, WEST),
};

std::vector<Pose2D> slamBottomLeft = {
    Pose2D(3, 3, WEST),
};

// set (6, 6, WEST)

std::vector<Pose2D> setupMainSweep = {
    Pose2D(20, 20, WEST),
    Pose2D(20, 20, EAST),
};

std::vector<Pose2D> mainSweep = {
    Pose2D(MAINSWEEPLEFTX + 3, 6, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 7, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6, EAST),

    Pose2D(MAINSWEEPLEFTX + 3, 6 * 2, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 6 * 2, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6 * 2, EAST),

    Pose2D(MAINSWEEPLEFTX + 3, 6 * 3, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 6 * 3, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6 * 3, EAST),

    Pose2D(MAINSWEEPLEFTX + 3, 6 * 4, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 6 * 4, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6 * 4, EAST),

    Pose2D(MAINSWEEPLEFTX + 3, 6 * 5, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 6 * 5, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6 * 5, EAST),

    Pose2D(MAINSWEEPLEFTX + 3, 6 * 6, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 6 * 6, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6 * 6, EAST),

    Pose2D(MAINSWEEPLEFTX + 3, 6 * 7, EAST),
    Pose2D(LEFTCAVEWALLX - 3, 6 * 7, EAST),
    Pose2D(MAINSWEEPLEFTX + 3, 6 * 7, EAST),
};
};  // namespace HardBox

namespace Paths {
std::vector<Pose2D> test_1 = {
    Pose2D(0, 30, 0.5 * PI),  // Wayp 1
    Pose2D(30, 30, 0.5 * PI)  // Wayp 2
};

std::vector<Pose2D> straight_back_40 = {
    Pose2D(0, 40, 0.5 * PI), Pose2D(0, 0, 0.5 * PI)  // Final Wayp
};

std::vector<Pose2D> side_left_40 = {
    Pose2D(-40, 0, 0.5 * PI), Pose2D(0, 0, 0.5 * PI)  // Final Wayp
};

std::vector<Pose2D> side_left_rotate_40 = {
    Pose2D(-40, 0, -0.5 * PI), Pose2D(0, 0, 0.5 * PI)  // Final Wayp
};

std::vector<Pose2D> andrew_waypoint = {
    Pose2D(STARTX, STARTY, 0.5 * PI), Pose2D(8, 27.5, 0.5 * PI),
    Pose2D(27.5, 41, 0.5 * PI),       Pose2D(51, 3, 0.5 * PI),
    Pose2D(40, 22.5, 0.5 * PI),       Pose2D(70, 22.5, 0.5 * PI),
    Pose2D(40, 22.5, 0.5 * PI),
};

std::vector<Pose2D> slam_left_corner = {
    Pose2D(3, 10, 0.5 * PI),
    Pose2D(3, 3, 0.5 * PI),
};

std::vector<Pose2D> beacon_position = {
    Pose2D(11.5, 28.5, 0.5 * PI),
};

std::vector<Pose2D> jostle_beacon = {
    Pose2D(11.5, 30.5, 0.5 * PI),
    Pose2D(11.5, 26.5, 0.5 * PI),
};

std::vector<Pose2D> beacon_pullout = {
    Pose2D(20, 28.5, 0.5 * PI),
};
}  // namespace Paths

#endif