/**
 * @file QuadEncoderObject.h
 * @brief Interfacing class for using the Quadrature Encoder library
 *
 * Defines the configuration structs QuadEncoderSetup and QuadEncoderObjet classes which manage a
 * single instsance of the QuadEncoder library object. Provides a standardized interface
 * @authors Aldem Pido
 * @section edit_history Edit history
 * 5/8/25 - Begin tracking
 */
#ifndef QUADENCODEROBJECT_h
#define QUADENCODEROBJECT_h

#include <Arduino.h>

#include <algorithm>
#include <memory>

#include "QuadEncoder.h"

/**
 * @ingroup encoder_types Encoder Types
 * @class QuadEncoderObject
 * @brief Interfacing class for a single instance of a Quadrature Encoder.
 *
 * Wraps a single intsance of the QuadEncoder library to provide a consistent interface.
 * Can print configuration and output messages.
 */
class QuadEncoderObject {
 public:
  /**
   * @struct QuadEncoderSetup
   * @brief Contains setup information for the QuadEncoderObject.
   */
  struct QuadEncoderSetup {
    int kENCA = -1;
    int kENCB = -1;
    int workMode = 0;  // 0 for normal use, 1 for FG and direction
    bool rev = false;
    /**
     * @brief Constructor for QuadEncoderSetup struct.
     * @param kA Signal pin 1.
     * @param kB Signal pin 2.
     * @param mode Work mode. 0 For normal use, 1 for FG and direction
     * @param rev Reverse encoder output.
     */
    constexpr QuadEncoderSetup(int kA, int kB, int mode = 0, bool rev = false)
        : kENCA(kA), kENCB(kB), workMode(mode), rev(rev) {}
  };
  explicit QuadEncoderObject(const QuadEncoderSetup &config, Print &output);
  virtual ~QuadEncoderObject() = default;
  void beginEnc();
  void updateEnc();
  long getEnc();
  void printInfo(Print &output, bool printConfig = false) const;
  friend Print &operator<<(Print &output, const QuadEncoderObject &enc);

 private:
  const QuadEncoderSetup &config;
  Print &output;
  std::unique_ptr<QuadEncoder> encoder;
  long enc;
  static int encoderNum;
};

#endif