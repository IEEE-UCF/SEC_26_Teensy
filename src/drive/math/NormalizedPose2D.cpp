#include "NormalizedPose2D.h"

/**
 * Defines a new NormalizedPose2D. The value of this is constrained to -1.0, 1.0.
 * This is a Pose2D only meant to represent unscaled velocity.
 */
NormalizedPose2D::NormalizedPose2D(float x, float y, float theta)
    : Pose2D(constrainValue(x), constrainValue(y), constrainValue(theta)) {}

float NormalizedPose2D::constrainValue(float value)
{
    return constrain(value, -1.0, 1.0);
}

/**
 * Rotate the pose around (0,0) as if it were a vector. Theta stays the same.
 *
 * @param angle Angle to rotate by
 */
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