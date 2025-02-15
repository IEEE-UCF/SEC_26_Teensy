#include "Pose2D.h"
#include "Arduino.h"

Pose2D::Pose2D(float x, float y, float rot) : x(x), y(y), rot(rot) {}

/*
    ChatGPT generated code
    Chaining multiple Pose2D objects in the main sketch
    Serial << pose1 << pose2 << pose3;
*/
Print &operator<<(Print &output, const Pose2D &pose)
{
    output.print(F("Pose: ("));
    output.print(pose.x);
    output.print(F(", "));
    output.print(pose.y);
    output.print(F(", "));
    output.print(pose.rot);
    output.println(F(")"));
    return output;
}
