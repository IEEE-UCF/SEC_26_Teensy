#include "Pose2D.h"
#include "Arduino.h"

Pose2D::Pose2D(float x, float y, float theta) : x(x), y(y), theta(theta) {}

/**
 * Adds one pose to another.
 *
 * @param pose Pose2D to add.
 */
Pose2D &Pose2D::add(const Pose2D &pose)
{
    x += pose.x;
    y += pose.y;
    theta += pose.theta;
    fixTheta();
    return *this; // Return a reference to the current object
}

/**
 * Reset pose to (0,0,0).
 */
Pose2D &Pose2D::reset()
{
    x = 0;
    y = 0;
    theta = 0;
    return *this; // Return a reference to the current object
}

/**
 * Translate vector
 *
 * @param dx Amount to change x by.
 * @param dy Amount to change y by.
 */
Pose2D &Pose2D::translate(float dx, float dy)
{
    x += dx;
    y += dy;
    return *this; // Return a reference to the current object
}

/**
 * Add to theta
 *
 * @param dtheta Amount to change theta by.
 */
Pose2D &Pose2D::rotate(float dtheta)
{
    theta += dtheta;
    fixTheta();
    return *this; // Return a reference to the current object
}

/**
 * Rotate the pose around (0,0) as if it were a vector. Theta stays the same
 *
 * @param angle Angle to rotate by
 */
Pose2D &Pose2D::rotateVector(float angle)
{
    float rad = radians(angle); // Convert angle to radians
    float cosA = cos(rad);
    float sinA = sin(rad);

    float newX = x * cosA - y * sinA;
    float newY = x * sinA + y * cosA;

    x = newX;
    y = newY;

    return *this; // Return a reference to the current object for chaining
}

/**
 * Check if theta overflows, and fix
 */

void Pose2D::fixTheta()
{
    while (theta > PI)
    {
        theta -= 2 * PI;
    }
    while (theta < -PI)
    {
        theta += 2 * PI;
    }
}

Print &operator<<(Print &output, const Pose2D &pose)
{
    output.print(F("Pose: ("));
    output.print(pose.x);
    output.print(F(", "));
    output.print(pose.y);
    output.print(F(", "));
    output.print(pose.theta);
    output.println(F(")"));
    return output;
}
