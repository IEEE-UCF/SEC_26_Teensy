/**
 * @file Pose2D.h
 * @brief Class for handling 2D Objects.
 * @authors Aldem Pido, Rafeed Khan
 */
#include "Pose2D.h"

#include <Arduino.h>
#include <math.h>

/**
 * @brief Creates a Pose2D. You can use this to represent position, velocity, or acceleration.
 * @param x Represents the x component.
 * @param y Reperesents the y component.
 * @param theta Represents the theta component if needed.
 * @param xymag Reperesnts the combined x and y magnitude if x and y are normalized to 1.
 */
Pose2D::Pose2D(float x, float y, float theta, float xymag)
    : x(x), y(y), theta(theta), xymag(xymag), normalized(false) {
  if (this->xymag < 1e-6f) this->xymag = 1.0f;
}

/**
 * @brief Normalizes magnitude of x and y to 1
 * @param scaleTheta Option to also scale theta.
 */
Pose2D &Pose2D::normalize(bool scaleTheta) {
  if (!normalized) {
    xymag = hypotf(x, y);
    constexpr float EPSILON = 1e-6f;
    if (xymag < EPSILON) {
      xymag = 1.0f;
      x = 0.0f;
      y = 0.0f;
      if (scaleTheta) theta = 0.0f;
    } else {
      x /= xymag;
      y /= xymag;
      if (scaleTheta) theta /= xymag;
    }
    normalized = true;
  }
  return *this;
}

/**
 * @brief Scales pose based on xymag
 * @param scaleTheta Option to also scale theta.
 */
Pose2D &Pose2D::unnormalize(bool scaleTheta) {
  if (normalized) {
    constexpr float MIN_XYMAG = 1e-3f;
    if (xymag < MIN_XYMAG) xymag = MIN_XYMAG;
    x *= xymag;
    y *= xymag;
    if (scaleTheta) theta *= xymag;
    xymag = 1.0f;
    normalized = false;
  }
  return *this;
}

/**
 * @brief Binds theta on +- pi bounds. Only use if you are working with position.
 */
Pose2D &Pose2D::fixTheta() {
  theta = fmod(theta, 2 * PI);
  if (theta >= PI)
    theta -= 2 * PI;
  else if (theta < -PI)
    theta += 2 * PI;
  return *this;
}

/**
 * @brief Constrains the x and y magnitude if x and y are normalized to 1.
 * @param magnitude magnitude of xymag
 */
Pose2D &Pose2D::constrainXyMag(float magnitude) {
  normalize();
  xymag = constrain(xymag, 0, magnitude);
  unnormalize();
  return *this;
}

/**
 * @brief Constrains theta.
 * @param magnitude Magnitude to constrain theta to.
 */
Pose2D &Pose2D::constrainTheta(float magnitude) {
  constrain(theta, -magnitude, magnitude);
  return *this;
}

/**
 * @brief Rotate vector.
 * @param angle Angle to rotate by radians.
 */
Pose2D &Pose2D::rotateVector(float angle) {
  const float cosA = cosf(angle);
  const float sinA = sinf(angle);
  return rotateVectorCached(cosA, sinA);
}

/**
 * @brief Rotate vector with cached angles.
 * @param cosAngle Cached value of cos.
 * @param sinAngle Cached value of sin.
 */
Pose2D &Pose2D::rotateVectorCached(float cosAngle, float sinAngle) {
  const float newX = x * cosAngle - y * sinAngle;
  const float newY = x * sinAngle + y * cosAngle;
  x = newX;
  y = newY;
  return *this;
}

/**
 * @brief Rotate (theta)
 * @param dtheta: Change in theta.
 */
Pose2D &Pose2D::rotate(float dtheta) {
  theta += dtheta;
  return *this;
}

/**
 * @brief Add a pose to current pose.
 * @param pose Pose to add.
 */
Pose2D &Pose2D::add(const Pose2D &pose) {
  x += pose.x;
  y += pose.y;
  theta += pose.theta;
  return *this;
}

/**
 * @brief Subtract a pose from current pose.
 * @param pose Pose to subtract.
 */
Pose2D &Pose2D::subtract(const Pose2D &pose) {
  x -= pose.x;
  y -= pose.y;
  theta -= pose.theta;
  return *this;
}

/**
 * @brief Multiply pose element by element.
 * @param pose Pose to mult element by element.
 */
Pose2D &Pose2D::multElement(const Pose2D &pose) {
  x *= pose.x;
  y *= pose.y;
  theta *= pose.theta;
  return *this;
}

/**
 * @brief Multiply all components by a scalar.
 * @param scalar
 */
Pose2D &Pose2D::multConstant(float scalar) {
  x *= scalar;
  y *= scalar;
  theta *= scalar;
  return *this;
}

/**
 * @brief Set all values to 0
 */
Pose2D &Pose2D::reset() {
  x = 0;
  y = 0;
  theta = 0;
  return *this;
}

/**
 * @brief Overloaded stream operator for printing pose details.
 * @param output Output stream.
 * @param pose Pose2D instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const Pose2D &pose) {
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
