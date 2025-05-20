/**
 * @file LineHandler.h
 * @defgroup sensors Line Sensor System
 * @brief Handles readings from line-following sensors.
 *
 * This group contains components for acquiring and processing data from line sensors.
 *
 * @author Aldem Pido
 */

#ifndef LINEHANDLER_H
#define LINEHANDLER_H

#include <Arduino.h>

/**
 * @class LineHandler
 * @ingroup sensors
 * @brief Manages line-following sensor readings.
 */
class LineHandler {
 public:
  LineHandler(int *kPins, int numPins);
  ~LineHandler();

  void Setup();
  void Update();
  void PrintInfo(Print &output, bool printConfig = false) const;

 private:
  int *kPins;       ///< Array of pin numbers for sensors
  int numPins;      ///< Number of sensors
  int *lineValues;  ///< Stores sensor readings
};

// Overloaded stream operator for printing sensor details.
Print &operator<<(Print &output, const LineHandler &handler);

#endif  // LINEHANDLER_H
