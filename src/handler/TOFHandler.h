/**
 * @file TOFHandler.h
 * @author Aldem Pido
 * @brief Defines the TOFHandler class for managing multiple VL53L0X Time-of-Flight sensors.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 * @defgroup sensors ToF Sensor System
 * This group contains components for interfacing with and managing Time-of-Flight distance sensors,
 * particularly when multiple sensors are used via an I2C multiplexer.
 */

#ifndef TOFHANDLER_H
#define TOFHANDLER_H

#include <Arduino.h>
#include <Print.h>
#include <VL53L0X.h>  // Adafruit VL53L0X library
#include <Wire.h>     // Required for I2C communication

#include "i2cmux.h"  // Custom I2C multiplexer library (ensure path is correct)

/**
 * @class TOFHandler
 * @ingroup sensors
 * @brief Manages multiple VL53L0X Time-of-Flight distance sensors using an I2C multiplexer.
 *  This class handles the initialization, update, and data retrieval for an array of
 * VL53L0X sensors, each connected to a different channel of an I2C multiplexer.
 */
class TOFHandler {
 public:
  TOFHandler(int *multiplexerChannels, int numSensors);  // Renamed params for clarity
  ~TOFHandler();

  bool Begin();
  void Update();
  const int *GetDistances() const;          // Renamed for clarity
  int GetDistanceAtIndex(int index) const;  // Renamed for clarity

  void PrintInfo(Print &output, bool printConfig = false) const;
  friend Print &operator<<(Print &output, const TOFHandler &handler);

 private:
  int *i2cMultiplexerChannels;  ///< Array of I2C multiplexer channel numbers for each sensor.
  int numManagedSensors;        ///< The number of TOF sensors being managed.
  VL53L0X *tofSensors;          ///< Dynamically allocated array of VL53L0X sensor objects.
  int *measuredDistances;  ///< Dynamically allocated array to store the latest distance from each
                           ///< sensor.
};

#endif  // TOFHANDLER_H