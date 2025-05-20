/**
 * @file Pose2D.h
 * @class Pose2D
 * @brief Class for handling 2D objects.
 *
 * This class can represent position, acceleration, or velocity. Users must track the type of value
 * stored. For example, `fixTheta()` should be used on a position vector but not on a velocity
 * vector. Many functions return `Pose2D`, allowing method chaining.
 *
 * @authors Aldem Pido, Rafeed Khan
 */
#ifndef Pose2D_H
#define Pose2D_H

#include <Arduino.h>

#include <cmath>

class Pose2D {
 public:
  Pose2D(float x = 0, float y = 0, float theta = 0, float xymag = 1);

  float getX() const { return x; }          ///< Get X component.
  float getY() const { return y; }          ///< Get Y component.
  float getTheta() const { return theta; }  ///< Get theta component.
  float getXyMag() const {
    return xymag;
  }  ///< Get the magnitude of the vector if x and y are normalized to 1.
  void setXyMag(float val) {
    xymag = val;
  }  ///< Set the magnitude of the vector if x and y are normalized to 1.
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