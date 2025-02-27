#include "Pose2D.h"
#include "Arduino.h"

Pose2D::Pose2D(float x, float y, float theta) : x(x), y(y), theta(theta) {}

Pose2D &Pose2D::add(const Pose2D &pose)
{
    x += pose.x;
    y += pose.y;
    theta += pose.theta;
    return *this; // Return a reference to the current object
}

Pose2D &Pose2D::reset()
{
    x = 0;
    y = 0;
    theta = 0;
    return *this; // Return a reference to the current object
}

Pose2D &Pose2D::translate(float dx, float dy)
{
    x += dx;
    y += dy;
    return *this; // Return a reference to the current object
}

Pose2D &Pose2D::rotate(float dtheta)
{
    theta += dtheta;
    return *this; // Return a reference to the current object
}

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
