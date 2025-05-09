
/*
Pose2D.cpp - Pose2D Object used to represent components in 2D space

Edit history:
5/8/24 - Begin tracking - Rafeed Khan, Aldem Pido
*/
#include "Pose2D.h"
#include <Arduino.h>
#include <math.h>

/**
 * Creates a Pose2D. Has an x, y, and theta. Use however you'd like
 */
Pose2D::Pose2D(float x, float y, float theta, float xymag)
    : x(x), y(y), theta(theta), xymag(xymag), normalized(false)
{
    if (this->xymag < 1e-6f)
        this->xymag = 1.0f;
}

/**
 * Normalizes magnitude of x and y to 1
 * @param scaleTheta option to also scale Theta
 */
Pose2D &Pose2D::normalize(bool scaleTheta)
{
    if (!normalized)
    {
        xymag = hypotf(x, y);
        constexpr float EPSILON = 1e-6f;
        if (xymag < EPSILON)
        {
            xymag = 1.0f;
            x = 0.0f;
            y = 0.0f;
            if (scaleTheta)
                theta = 0.0f;
        }
        else
        {
            x /= xymag;
            y /= xymag;
            if (scaleTheta)
                theta /= xymag;
        }
        normalized = true;
    }
    return *this;
}

/**
 * Scales pose based on xymag
 * @param scaleTheta option to also scale Theta
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
 * Binds theta on +- pi bounds, ideal for position
 */
Pose2D &Pose2D::fixTheta()
{
    theta = fmod(theta, 2 * PI);
    if (theta >= PI)
        theta -= 2 * PI;
    else if (theta < -PI)
        theta += 2 * PI;
    return *this;
}

/**
 * Constrains xymag.
 * @param magnitude magnitude of xymag
 */
Pose2D &Pose2D::constrainXyMag(float magnitude)
{
    normalize();
    xymag = constrain(xymag, 0, magnitude);
    unnormalize();
    return *this;
}

/**
 * Constrains theta.
 * @param magnitude magnitude of theta
 */
Pose2D &Pose2D::constrainTheta(float magnitude)
{
    constrain(theta, -magnitude, magnitude);
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
 * Rotate (theta)
 * @param dtheta: change in theta
 */
Pose2D &Pose2D::rotate(float dtheta)
{
    theta += dtheta;
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

Print &operator<<(Print &output, const Pose2D &pose)
{
    // float thetaDeg = degrees(pose.theta);
    output.print(F("Pose: ("));
    output.print(pose.x);
    output.print(F(", "));
    output.print(pose.y);
    output.print(F(", "));
    /*output.print(thetaDeg);
    output.println(F("°)"));*/
    output.print(pose.theta);
    output.println(F(")"));
    return output;
}
