#include "HallHandler.h"

#include <Arduino.h>

/**
 * @brief Constructs a HallHandler object.
 * @param kPins Array of hall sensor pins.
 * @param numPins Number of sensors.
 */
HallHandler::HallHandler(int *kPins, int numPins) {
  this->kPins = kPins;
  this->numPins = numPins;
  analogValues = new int[numPins];
}

/**
 * @brief Destructor for HallHandler, releasing allocated memory.
 */
HallHandler::~HallHandler() { delete[] analogValues; }

/**
 * @brief Initializes sensor pins and sets analog read resolution.
 */
void HallHandler::Begin() {
  analogReadResolution(8);
  analogReadAveraging(32);
  for (int i = 0; i < numPins; i++) {
    pinMode(kPins[i], INPUT);
  }
}

/**
 * @brief Reads hall sensor values and updates stored data.
 */
void HallHandler::Update() {
  for (int i = 0; i < numPins; i++) {
    analogValues[i] = analogRead(kPins[i]);
  }
}

/**
 * @brief Retrieves the current hall sensor readings.
 * @return Pointer to an array containing sensor values.
 */
int *HallHandler::getReadings() const { return analogValues; }

/**
 * @brief Prints hall sensor configuration or current readings.
 * @param output Output stream for logging.
 * @param printConfig If true, prints sensor configuration; otherwise, prints current readings.
 */
void HallHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("HallHandler Configuration: "));
    output.print(F("Number of Pins: "));
    output.println(numPins);
    output.print(F("Pins: "));
    for (int i = 0; i < numPins; i++) {
      output.print(kPins[i]);
      if (i < numPins - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  } else {
    output.print(F("Analog Values: "));
    for (int i = 0; i < numPins; i++) {
      output.print(analogValues[i]);
      if (i < numPins - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  }
}

/**
 * @brief Overloaded stream operator for printing hall sensor details.
 * @param output Output stream.
 * @param handler HallHandler instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const HallHandler &handler) {
  handler.PrintInfo(output, false);
  return output;
}
