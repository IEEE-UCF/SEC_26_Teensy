#include "Pose2D.h"
#include <Arduino.h>
#include <math.h>

/**
 * Creates a Pose2D. Has an x, y, and theta. Use however you'd like
 */
Pose2D::Pose2D(float x, float y, float theta, float xymag, bool debug)
    : x(x), y(y), theta(theta), xymag(xymag), normalized(false), debugMode(debug)
{
    if (this->xymag < 1e-6f)
        this->xymag = 1.0f;
    fixTheta();
}

/**
 * Normalizes magnitude of x and y to 1
 */
Pose2D &Pose2D::normalize()
{
    if (!normalized)
    {
        xymag = hypotf(x, y);
        constexpr float EPSILON = 1e-6f;
        if (xymag < EPSILON)
        {
            xymag = 1.0f;
            x = 1.0f;
            y = 0.0f;
        }
        else
        {
            x /= xymag;
            y /= xymag;
        }
        normalized = true;
    }
    return *this;
}

/**
 * Binds theta on pi bounds
 */
Pose2D &Pose2D::fixTheta()
{
    theta = fmod(theta, 2 * PI);
    while (theta >= PI)
        theta -= 2 * PI;
    while (theta < -PI)
        theta += 2 * PI;
    return *this;
}

/**
 * Noramlize with pi bounds
 */
Pose2D &Pose2D::fullNormalize()
{
    normalize();
    fixTheta();
    return *this;
}

/**
 * Unnormalizes pose
 */
Pose2D &Pose2D::unnormalize(bool scaleTheta)
{
    if (normalized)
    {
        constexpr float MIN_XYMAG = 1e-3f;
        if (xymag < MIN_XYMAG)
            xymag = MIN_XYMAG;

        x *= xymag;
        y *= xymag;
        if (scaleTheta)
            theta *= xymag;

        xymag = 1.0f;
        normalized = false;
    }
    return *this;
}

/**
 * Rotate vector
 * @param angle angle to rate by in radians
 */
Pose2D &Pose2D::rotateVector(float angle)
{
    const float cosA = cosf(angle);
    const float sinA = sinf(angle);
    return rotateVectorCached(cosA, sinA);
}

/**
 * Rotate vector with cached angles
 * @param cosAngle cached value of cos
 * @param sinAngle cached value of sin
 */
Pose2D &Pose2D::rotateVectorCached(float cosAngle, float sinAngle)
{
    const float newX = x * cosAngle - y * sinAngle;
    const float newY = x * sinAngle + y * cosAngle;
    x = newX;
    y = newY;
    return *this;
}

/**
 * Add a pose to current pose.
 * @param pose pose to add
 */
Pose2D &Pose2D::add(const Pose2D &pose)
{
    x += pose.x;
    y += pose.y;
    theta += pose.theta;
    fixTheta();
    return *this;
}

/**
 * Subtract a pose from current pose
 * @param pose pose to subtract
 */
Pose2D &Pose2D::subtract(const Pose2D &pose)
{
    x -= pose.x;
    y -= pose.y;
    theta -= pose.theta;
    fixTheta();
    return *this;
}

/**
 * Multiply pose element by element
 * @param pose pose to mult element by element
 */
Pose2D &Pose2D::multElement(const Pose2D &pose)
{
    x *= pose.x;
    y *= pose.y;
    theta *= pose.theta;
    return *this;
}

/**
 * Multiply all components by a scalar
 * @param pose scalar amount
 */
Pose2D &Pose2D::multConstant(float scalar)
{
    x *= scalar;
    y *= scalar;
    theta *= scalar;
    return *this;
}

/**
 * Set all values to 0
 */
Pose2D &Pose2D::reset()
{
    x = 0;
    y = 0;
    theta = 0;
    return *this;
}

/**
 * Translate pose (x, y)
 * @param dx: change in x
 * @param dy: change in y
 */
Pose2D &Pose2D::translate(float dx, float dy)
{
    x += dx;
    y += dy;
    return *this;
}

/**
 * Rotate (theta)
 * @param dtheta: change in theta
 */
Pose2D &Pose2D::rotate(float dtheta)
{
    theta += dtheta;
    fixTheta();
    return *this;
}

Print &operator<<(Print &output, const Pose2D &pose)
{
    float thetaDeg = degrees(pose.theta);
    output.print(F("Pose: ("));
    output.print(pose.x);
    output.print(F(", "));
    output.print(pose.y);
    output.print(F(", "));
    output.print(thetaDeg);
    output.println(F("°)"));
    return output;
}
