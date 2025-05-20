/**
 * @file RCHandler.cpp
 * @author Aldem Pido, Tino Hernandez
 * @brief Implements the RCHandler class for processing IBUS remote control signals.
 * @date May 19, 2025 // Assuming current date for last modification
 */
#include "RCHandler.h"

#include <Arduino.h>  // For map, constrain, Print, F()

/**
 * @brief Constructs an RCHandler object.
 *  Initializes the `updateTimer` and sets all channel values in the `pots` array to 0.
 * This prepares the handler for communication without initial garbage values.
 */
RCHandler::RCHandler() : updateTimer(0) {
  for (int i = 0; i < 10; i++) {
    pots[i] = 0;  // Initialize all potential channel values
  }
}

/**
 * @brief Initializes IBUS communication.
 *  Starts the IBUS communication on the specified hardware serial port.
 * It configures the IBusBM library to operate without its internal timer,
 * as timing is handled by the `updateTimer` in the `Update` method.
 * @param serial A reference to the HardwareSerial object (e.g., Serial1, Serial2)
 * to be used for IBUS communication.
 */
void RCHandler::Begin(HardwareSerial &serial) {
  IBus.begin(serial,
             IBUSBM_NOTIMER);  // Initialize IBus on the given serial port without internal timer
}

/**
 * @brief Reads and updates the values from the FlySky receiver.
 *  This method should be called periodically. It checks if the `DELAY`
 * period has elapsed since the last update. If so, it calls `IBus.loop()` to
 * read new data from the receiver. It then reads up to 10 channels, maps the raw
 * IBUS values (typically 1000-2000) to a bipolar range (-255 to 255), and stores
 * them. A simple failsafe mechanism is implemented: if channel 9 (index) reads -255
 * (often a switch indicating off or failsafe), channels 0, 1, 2, 3, and 5 are zeroed out.
 * Otherwise, all read channel values are updated.
 */
void RCHandler::Update() {
  if (updateTimer >= DELAY) {  // Check if it's time to update
    updateTimer -= DELAY;      // Reset the timer by subtracting the delay (allows for drift)
    IBus.loop();               // Process incoming IBUS data

    int32_t newPots[10];
    for (int i = 0; i < 10; i++) {
      // Read raw channel data (1000-2000), constrain, then map to -255 to 255
      newPots[i] = map(constrain(IBus.readChannel(i), 1000, 2000) - 1000, 0, 1000, -255, 255);
    }

    // Failsafe/mode check: if channel 9 is at its minimum (-255 after mapping),
    // zero out specific control channels. This is often used for a "kill switch" or "disarm"
    // function.
    if (newPots[9] == -255) {
      pots[0] = 0;     // e.g., Roll/Aileron
      pots[1] = 0;     // e.g., Pitch/Elevator
      pots[2] = 0;     // e.g., Throttle
      pots[3] = 0;     // e.g., Yaw/Rudder
      pots[5] = 0;     // e.g., Auxiliary channel
      pots[9] = -255;  // Keep failsafe switch state
    } else {
      // If not in failsafe, update all channels with new readings
      for (int i = 0; i < 10; i++) {
        pots[i] = newPots[i];
      }
    }
  }
}

/**
 * @brief Retrieves the scaled value of a specific RC channel.
 *  Returns the processed value for the requested channel index.
 * The value ranges from -255 to 255.
 * @param channel The channel index to retrieve (0 to 9).
 * @return The scaled value of the specified channel. Returns 0 if the channel index
 * is out of bounds.
 */
int32_t RCHandler::Get(int channel) {
  if (channel >= 0 && channel < 10) {
    return pots[channel];
  }
  return 0;  // Return 0 for invalid channel index
}

/**
 * @brief Prints RCHandler configuration or current input values to a Print stream.
 *  If `printConfig` is true, it prints static configuration information like
 * the number of channels handled. Otherwise (if `printConfig` is false), it prints
 * the current scaled values of all 10 RC channels.
 * @param output Reference to a Print object (e.g., Serial, LCD) where the info will be printed.
 * @param printConfig Boolean flag; if true, prints configuration details,
 * else prints current runtime channel values. Defaults to false.
 */
void RCHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.println(F("RCHandler Configuration:"));
    output.print(F("Number of Channels: "));
    output.println(10);  // Fixed to 10 channels based on implementation
    output.print(F("Update Delay (ms): "));
    output.println(DELAY);
  } else {
    output.print(F("RC Channel States: ["));
    for (int i = 0; i < 10; i++) {
      output.print(pots[i]);
      if (i < 9) {
        output.print(F(", "));
      }
    }
    output.println(F("]"));
  }
}

/**
 * @brief Overloaded stream insertion operator for printing RCHandler details.
 *  Allows an RCHandler object to be easily printed to any Print-derived stream (like Serial)
 * using the `<<` operator. This will print the current runtime channel values by default.
 * Example: `Serial << myRCHandler;`
 * @param output Reference to the Print stream.
 * @param handler Const reference to the RCHandler object to print.
 * @return Reference to the Print stream, allowing for chaining.
 */
Print &operator<<(Print &output, const RCHandler &handler) {
  handler.PrintInfo(output, false);  // Defaults to printing runtime values
  return output;
}