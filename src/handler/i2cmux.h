/**
 * @file i2cmux.h
 * @defgroup i2c I2C Multiplexer
 * @brief Handles communication with the TCA9548A/PCA9548A I2C multiplexer.
 *
 * This group provides functions for selecting I2C channels using the multiplexer.
 *
 * @author Aldem Pido
 */

#ifndef I2CMUX_H
#define I2CMUX_H

#include <Wire.h>

#define TCAADDR 0x70  ///< I2C address of the multiplexer flag

/**
 * @namespace i2cmux
 * @ingroup i2c
 * @brief Provides functions for controlling the I2C multiplexer.
 */
namespace i2cmux {
void tcaselect(uint8_t i);
}

#endif
