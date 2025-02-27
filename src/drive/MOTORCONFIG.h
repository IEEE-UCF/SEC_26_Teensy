#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#define TRACK_WIDTH 10      // inches, distance from encoder wheel to encoder wheel
#define WHEEL_DIAMETER 3.25 // inches

#define MOTOR_1_1 2700         // rpm, no gears
#define GEARRATIO 34           // gear ratio
#define TICKS_PER_REVOLUTION 3 // encoder ticks per revolution

#define MAX_VELOCITY 10    // inches per second
#define MAX_ACCELERATION 5 // inches per second^2

// Wheel Offsets
#define WHEEL_OFFSET_Y 2 // inches, offset of the left and right wheels from the center
#define BACK_OFFSET_F 4  // inches, offset of the back wheel from the center

constexpr float WHEEL_CIRCUMFERENCE = PI * WHEEL_DIAMETER;

#endif
