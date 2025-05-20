/**
 * @file LightHandler.h
 * @defgroup sensors Light Sensor System
 * @brief Handles readings from the BH1750 light sensor.
 *
 * This group contains components for acquiring and processing light sensor data.
 *
 * @author Aldem Pido
 */

#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H

#include <BH1750.h>
#include <Wire.h>

#include "i2cmux.h"

/**
 * @class LightHandler
 * @ingroup sensors
 * @brief Manages BH1750 light sensor readings.
 */
class LightHandler {
 public:
  LightHandler(int cLight);
  bool Begin();
  void Update();
  void PrintInfo(Print &output, bool printConfig = false) const;
  float GetLightLevel() const;

 private:
  int cLight;         ///< Channel index for I2C multiplexer
  BH1750 lightMeter;  ///< BH1750 light sensor instance
  float lightLevel;   ///< Stores the latest light level measurement
};

// Overloaded stream operator for printing sensor details.
Print &operator<<(Print &output, const LightHandler &handler);

#endif  // LIGHTHANDLER_H
