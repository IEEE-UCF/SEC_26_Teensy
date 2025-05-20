/**
 * @file TOFHandler.cpp
 * @author Aldem Pido
 * @brief Implements the TOFHandler class for VL53L0X sensor array management.
 * @date April 1, 2025 (original), Last Modified: May 19, 2025
 */
#include "TOFHandler.h"

#include <Arduino.h>  // For Serial, F()

/**
 * @brief Constructs a TOFHandler object.
 *  Stores the I2C multiplexer channel assignments and the number of sensors.
 * Dynamically allocates memory for sensor objects and distance readings.
 * @param multiplexerChannels Pointer to an array of integers, where each integer is the
 * I2C multiplexer channel for the corresponding sensor.
 * @param numSensors The total number of VL53L0X sensors to manage.
 */
TOFHandler::TOFHandler(int *multiplexerChannels, int numSensors) {
  this->i2cMultiplexerChannels = multiplexerChannels;
  this->numManagedSensors = numSensors;
  tofSensors = new VL53L0X[numManagedSensors];
  measuredDistances = new int[numManagedSensors];
}

/**
 * @brief Destructor for TOFHandler.
 *  Frees the dynamically allocated memory for sensor objects and distance arrays
 * to prevent memory leaks.
 */
TOFHandler::~TOFHandler() {
  delete[] tofSensors;
  delete[] measuredDistances;
}

/**
 * @brief Initializes all configured VL53L0X sensors.
 *  Iterates through each sensor, selects its I2C multiplexer channel,
 * sets the I2C bus (Wire1), and initializes the sensor. Configures sensor
 * parameters like signal rate limit, measurement timing budget, and VCSEL pulse periods.
 * Starts continuous measurement mode for all sensors.
 * @return True if all sensors were successfully initialized, false if any sensor fails.
 */
bool TOFHandler::Begin() {
  bool success = true;
  for (int i = 0; i < numManagedSensors; i++) {
    i2cmux::tcaselect(i2cMultiplexerChannels[i]);  // Select I2C mux channel
    tofSensors[i].setBus(&Wire1);                  // Assuming sensors are on I2C bus Wire1
    // sensors[i].setTimeout(500); // Optional: set sensor timeout
    if (!tofSensors[i].init()) {
      Serial.print(F("Failed to detect and initialize sensor at mux channel "));
      Serial.println(i2cMultiplexerChannels[i]);
      success = false;  // Mark as failed but continue trying others
    }
    // Configure sensor parameters for potentially better performance/accuracy
    tofSensors[i].setSignalRateLimit(0.10f);          // Set signal rate limit
    tofSensors[i].setMeasurementTimingBudget(20000);  // Set timing budget in microseconds
    tofSensors[i].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange,
                                      18);  // Set VCSEL pre-range pulse period
    tofSensors[i].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange,
                                      14);  // Set VCSEL final-range pulse period
    tofSensors[i].startContinuous();        // Start continuous ranging mode
  }
  return success;
}

/**
 * @brief Updates distance readings from all sensors.
 *  For each sensor, selects its I2C multiplexer channel and reads the
 * latest distance measurement in continuous mode. Stores the reading in millimeters.
 * If a timeout occurs during a read, a message is printed to Serial.
 */
void TOFHandler::Update() {
  for (int i = 0; i < numManagedSensors; i++) {
    i2cmux::tcaselect(i2cMultiplexerChannels[i]);
    measuredDistances[i] = tofSensors[i].readRangeContinuousMillimeters();
    if (tofSensors[i].timeoutOccurred()) {
      Serial.print(F("TOF Sensor Timeout: Mux Channel "));
      Serial.println(i2cMultiplexerChannels[i]);
    }
  }
}

/**
 * @brief Gets a pointer to the array of current distance readings.
 *  Provides access to all measured distances. The distances are in millimeters.
 * @return A const pointer to the integer array containing the latest distance
 * reading for each sensor.
 */
const int *TOFHandler::GetDistances() const { return measuredDistances; }

/**
 * @brief Gets the distance reading for a specific sensor by its index.
 *  Retrieves the last measured distance for the sensor at the given index.
 * @param index The index of the sensor (0 to numManagedSensors-1).
 * @return The distance in millimeters for the specified sensor. Returns -1 if the
 * index is out of bounds.
 */
int TOFHandler::GetDistanceAtIndex(int index) const {
  if (index >= 0 && index < numManagedSensors) {
    return measuredDistances[index];
  }
  return -1;  // Indicate an error or out-of-bounds index
}

/**
 * @brief Prints TOFHandler configuration or current distance readings.
 *  If `printConfig` is true, outputs the number of channels and their
 * I2C multiplexer assignments. Otherwise, prints the latest distance reading
 * for each sensor.
 * @param output Reference to a Print object (e.g., Serial) for output.
 * @param printConfig If true, print configuration; otherwise, print current data.
 */
void TOFHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.println(F("TOFHandler Configuration:"));
    output.print(F("  Number of Sensors: "));
    output.println(numManagedSensors);
    output.print(F("  Multiplexer Channels: "));
    for (int i = 0; i < numManagedSensors; i++) {
      output.print(i2cMultiplexerChannels[i]);
      if (i < numManagedSensors - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  } else {
    output.print(F("TOF Distances (mm): ["));
    for (int i = 0; i < numManagedSensors; i++) {
      output.print(measuredDistances[i]);
      if (i < numManagedSensors - 1) {
        output.print(F(", "));
      }
    }
    output.println(F("]"));
  }
}

/**
 * @brief Overloaded stream insertion operator for printing TOFHandler distances.
 *  Allows easy printing of the current sensor distances to a Print stream.
 * Example: `Serial << myTOFHandler;`
 * @param output Reference to the Print stream.
 * @param handler Const reference to the TOFHandler object.
 * @return Reference to the Print stream.
 */
Print &operator<<(Print &output, const TOFHandler &handler) {
  handler.PrintInfo(output, false);  // Default to printing current distances
  return output;
}