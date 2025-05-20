#include "ButtonHandler.h"

#include <Arduino.h>

/**
 * @brief Constructs a ButtonHandler object.
 * @param kPins Array of button pins.
 * @param numPins Number of buttons.
 */
ButtonHandler::ButtonHandler(int *kPins, int numPins) {
  this->kPins = kPins;
  this->numPins = numPins;
  buttonStates = new bool[numPins];
}

/**
 * @brief Initializes button pins as input with pull-up resistors. (testflag 999)
 */
void ButtonHandler::Begin() {
  for (int i = 0; i < numPins; i++) {
    pinMode(kPins[i], INPUT_PULLUP);
  }
}

/**
 * @brief Reads button states and updates stored values.
 */
void ButtonHandler::Update() {
  for (int i = 0; i < numPins; i++) {
    buttonStates[i] = digitalRead(kPins[i]) == LOW;
  }
}

/**
 * @brief Retrieves the current button states.
 * @return Pointer to an array of boolean values (`true` = pressed, `false` = released).
 */
bool *ButtonHandler::GetStates() const { return buttonStates; }

/**
 * @brief Prints button information.
 * @param output Output stream for logging.
 * @param printConfig If true, prints configuration details; otherwise, prints runtime states.
 */
void ButtonHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("ButtonHandler Configuration: "));
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
    output.print(F("Button States: "));
    for (int i = 0; i < numPins; i++) {
      output.print(buttonStates[i] ? F("Pressed") : F("Released"));
      if (i < numPins - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  }
}

/**
 * @brief Overloaded stream operator for printing button details.
 * @param output Output stream.
 * @param handler ButtonHandler instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const ButtonHandler &handler) {
  handler.PrintInfo(output, false);
  return output;
}
