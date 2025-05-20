/**
 * @file ButtonHandler.h
 * @defgroup inputs Button Input System
 * @brief Handles button and switch inputs.
 *
 * This group contains components for managing button states and handling user interactions.
 *
 * @author Aldem Pido
 */

#ifndef ButtonHandler_h
#define ButtonHandler_h

#include <Arduino.h>

/**
 * @class ButtonHandler
 * @ingroup inputs
 * @brief Manages button input states.
 */
class ButtonHandler {
 public:
  ButtonHandler(int *kPins, int numPins);
  void Begin();
  void Update();
  bool *GetStates() const;

  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const ButtonHandler &handler);

 private:
  bool *buttonStates;  ///< Stores button press states
  int *kPins;          ///< Array of pin numbers for buttons
  int numPins;         ///< Number of buttons
};

#endif
