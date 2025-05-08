/*
Pose2D.h - Pose2D Object used to represent components in 2D space

Description:
Can be used to represent any type of value like position, acceleration, or velocity.
You will have to keep track of what type of value you are storing. For example,
you would have to use fixTheta() on a position vector, but you wouldn't use it on a
velocity vector. You can also chain together Pose2D as a lot of the functions return
themselves.

Edit history:
4/1/25 - Begin tracking - Rafeed Khan, Aldem Pido
5/8/25 - Workflow changes - Aldem Pido
*/
#ifndef Pose2D_H
#define Pose2D_H

#include <Arduino.h>
#include <cmath>

class Pose2D
{
public:
    Pose2D(float x = 0, float y = 0, float theta = 0, float xymag = 1);

    float getX() const { return x; }
    float getY() const { return y; }
    float getTheta() const { return theta; }
    float getXyMag() const { return xymag; }
    void setXyMag(float val) { xymag = val; }
    /*
    Removed as using these functions results in bad overall workflow. Create a new Pose2D
    instead of setting these. Use reset and add if necessary
    void setX(float val) { x = val; }
    void setY(float val) { y = val; }
    void setThetaRadians(float val) { theta = val; }
    void setThetaDegrees(float deg) { theta = radians(deg); }*/

    // Normalization, constrains
    Pose2D &normalize(bool scaleTheta = false);
    Pose2D &unnormalize(bool scaleTheta = false);
    Pose2D &fixTheta();
    Pose2D &constrainXyMag(float magnitude);
    Pose2D &constrainTheta(float magnitude);

    // Operations
    Pose2D &rotateVector(float angle);
    Pose2D &rotateVectorCached(float cosAngle, float sinAngle);
    Pose2D &rotate(float dtheta);
    Pose2D &add(const Pose2D &pose);
    Pose2D &subtract(const Pose2D &pose);
    Pose2D &multElement(const Pose2D &pose);
    Pose2D &multConstant(float scalar);
    Pose2D &reset();

    friend Print &operator<<(Print &output, const Pose2D &pose);

private:
    float x, y, theta;
    float xymag;
    bool normalized;
};
#endif