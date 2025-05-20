#include "LightHandler.h"

/**
 * @brief Constructs a LightHandler object.
 * @param cLight Channel index for the I2C multiplexer.
 */
LightHandler::LightHandler(int cLight) : cLight(cLight), lightMeter(), lightLevel(0.0) {}

/**
 * @brief Initializes the BH1750 light sensor.
 * @return True if initialization succeeds, false otherwise.
 */
bool LightHandler::Begin() {
  i2cmux::tcaselect(cLight);

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1)) {
    Serial.println("Light sensor initialized");
    return true;
  } else {
    Serial.println("Error initializing light sensor");
    return false;
  }
}

/**
 * @brief Reads and updates the light sensor data.
 */
void LightHandler::Update() {
  i2cmux::tcaselect(cLight);
  lightLevel = lightMeter.readLightLevel();
}

/**
 * @brief Prints light sensor configuration or current readings.
 * @param output Output stream for logging.
 * @param printConfig If true, prints sensor configuration; otherwise, prints current readings.
 */
void LightHandler::PrintInfo(Print &output, bool printConfig) const {
  output.print("Light Level: ");
  output.print(lightLevel);
  output.println(" lx");

  if (printConfig) {
    output.println("Configuration:");
    output.println("Mode: Continuous High Resolution");
  }
}

/**
 * @brief Retrieves the current light level measurement.
 * @return Light level in lux.
 */
float LightHandler::GetLightLevel() const { return lightLevel; }

/**
 * @brief Overloaded stream operator for printing light sensor details.
 * @param output Output stream.
 * @param handler LightHandler instance.
 * @return Modified output stream.
 */
Print &operator<<(Print &output, const LightHandler &handler) {
  handler.PrintInfo(output);
  return output;
}
