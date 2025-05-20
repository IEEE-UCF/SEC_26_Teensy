#include "LineHandler.h"

/**
 * @brief Constructs a LineHandler object.
 * @param kPins Array of sensor pins.
 * @param numPins Number of sensors.
 */
LineHandler::LineHandler(int *kPins, int numPins) {
  this->kPins = kPins;
  this->numPins = numPins;
  lineValues = new int[numPins];
}

/**
 * @brief Destructor for LineHandler, releasing allocated memory.
 */
LineHandler::~LineHandler() { delete[] lineValues; }

/**
 * @brief Initializes sensor pins.
 */
void LineHandler::Setup() {
  for (int i = 0; i < numPins; i++) {
    pinMode(kPins[i], INPUT);
  }
}

/**
 * @brief Reads sensor values and updates stored data.
 */
void LineHandler::Update() {
  for (int i = 0; i < numPins; i++) {
    lineValues[i] = analogRead(kPins[i]);
  }
}

/**
 * @brief Prints line sensor configuration or current readings.
 * @param output Output stream for logging.
 * @param printConfig If true, prints sensor configuration; otherwise, prints current readings.
 */
void LineHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("LineHandler Configuration: "));
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
    output.print(F("Line Values: "));
    for (int i = 0; i < numPins; i++) {
      output.print(lineValues[i]);
      if (i < numPins - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  }
}

/**
 * @brief Overloaded stream operator for printing line sensor details.
 * @param output Output stream.
 * @param handler LineHandler instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const LineHandler &handler) {
  handler.PrintInfo(output, false);
  return output;
}
