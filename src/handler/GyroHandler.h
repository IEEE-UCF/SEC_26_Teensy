/**
 * @file GyroHandler.h
 * @defgroup sensors Gyro and Sensor Systems
 * @brief Handles readings from the BNO08x sensor.
 *
 * This group contains components for acquiring and processing gyro sensor data.
 *
 * @author Aldem Pido
 */

#ifndef GYROHANDLER_H
#define GYROHANDLER_H

#include <Adafruit_BNO08x.h>
#include <Wire.h>

/**
 * @class GyroHandler
 * @ingroup sensors
 * @brief Reads gyro sensor data using the BNO08x sensor.
 */
class GyroHandler {
 public:
  GyroHandler();
  bool Begin();
  void Update();
  void PrintInfo(Print &output, bool printConfig = false) const;
  void Set_Gametime_Offset(float angleRad) { Gametime_Offset = angleRad - BEGIN_OFFSET * PI / 180; }
  float *GetGyroData();

 private:
  Adafruit_BNO08x bno08x;         ///< BNO08x gyro sensor instance
  sh2_SensorValue_t sensorValue;  ///< Stores sensor event data
  float gyroData[3];              ///< Array containing yaw, pitch, and roll values
  float Gametime_Offset;          ///< Offset for angle adjustments
};

// Overloaded stream operator for printing gyro information
Print &operator<<(Print &output, const GyroHandler &handler);

#endif  // GYROHANDLER_H
