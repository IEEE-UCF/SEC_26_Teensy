#include "NormalizedPose2D.h"

NormalizedPose2D::NormalizedPose2D(float x, float y, float theta)
    : Pose2D(constrainValue(x), constrainValue(y), constrainValue(theta)) {}

float NormalizedPose2D::constrainValue(float value)
{
    return constrain(value, -1.0, 1.0);
}

NormalizedPose2D &NormalizedPose2D::rotateVector(float angle)
{
    float rad = radians(angle); // Convert angle to radians
    float cosA = cos(rad);
    float sinA = sin(rad);

    float newX = x * cosA - y * sinA;
    float newY = x * sinA + y * cosA;

    x = constrainValue(newX);
    y = constrainValue(newY);

    return *this; // Return a reference to the current object for chaining
}

Print &operator<<(Print &output, const NormalizedPose2D &pose)
{
    output.print(F("Normalized Pose: ("));
    output.print(pose.x);
    output.print(F(", "));
    output.print(pose.y);
    output.print(F(", "));
    output.print(pose.theta);
    output.println(F(")"));
    return output;
}