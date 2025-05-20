/**
 * @file RCHandler.h
 * @author Aldem Pido, Tino Hernandez
 * @brief Defines the RCHandler class for interfacing with FlySky receivers using the IBUS protocol.
 * @date May 19, 2025 // Assuming current date for last modification
 * @defgroup communication Remote Control System
 * This group contains components for managing remote control inputs.
 */

#ifndef RCHANDLER_H
#define RCHANDLER_H

#include <IBusBM.h>
#include <Print.h>  // Added for Print &operator<<
#include <elapsedMillis.h>

#define DELAY 2  ///< Delay in milliseconds for updating input values from the receiver.

/**
 * @class RCHandler
 * @ingroup communication
 * @brief Handles FlySky IBUS remote control input.
 *  This class initializes and manages communication with a FlySky receiver
 * via the IBUS protocol. It reads channel data, scales it, and provides access
 * to these values. It also includes functionality for failsafe behavior if a
 * specific channel indicates a signal loss or specific state.
 */
class RCHandler {
 public:
  RCHandler();
  void Begin(HardwareSerial &serial);
  void Update();
  int32_t Get(int channel);
  void PrintInfo(Print &output, bool printConfig = false) const;  // Added default for printConfig
  friend Print &operator<<(Print &output, const RCHandler &handler);

 private:
  IBusBM IBus;                ///< IBUS interface object for communication with the receiver.
  int32_t pots[10];           ///< Array storing scaled input values for up to 10 channels.
  elapsedMillis updateTimer;  ///< Timer to manage the frequency of IBUS updates.
};

#endif  // RCHANDLER_H