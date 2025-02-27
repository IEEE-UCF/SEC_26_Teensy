#include "NormalizedPose2D.h"

NormalizedPose2D::NormalizedPose2D(float x, float y, float rot)
    : x(constrainValue(x)), y(constrainValue(y)), rot(constrainValue(rot)) {}

float NormalizedPose2D::constrainValue(float value)
{
    return constrain(value, -1.0, 1.0);
}

Print &operator<<(Print &output, const NormalizedPose2D &pose)
{
    output.print(F("Normalized Pose: ("));
    output.print(pose.x);
    output.print(F(", "));
    output.print(pose.y);
    output.print(F(", "));
    output.print(pose.rot);
    output.println(F(")"));
    return output;
}
