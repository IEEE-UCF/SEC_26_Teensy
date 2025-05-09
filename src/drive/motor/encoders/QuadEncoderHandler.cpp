/*
QuadEncoderHandler.cpp - Interfacing class for using the Quadrature Encoder
library

Edit history
5/8/25 - Begin tracking - Aldem Pido
*/
#include "QuadEncoderHandler.h"
const int validEncoderPins[] = {0, 1, 2, 3, 4, 5, 7, 8, 30, 31, 33};
const size_t numValidEncoderPins =
    sizeof(validEncoderPins) / sizeof(validEncoderPins[0]);
int QuadEncoderHandler::encoderNum = 1;

/**
 * Standard way to check if pin is contained in valid pin list.
 */
bool pinCheck(int pin, const int *pinList, size_t listSize) {
  if (pin == -1)  // Check for sentinel value
    return false;
  return std::find(pinList, pinList + listSize, pin) !=
         (pinList + listSize);  // Use C++ function
}

/**
 * Normal constructor for quad encoder.
 * @param config Setup pins for the encoder.
 * @param output Pass Serial in here.
 */
QuadEncoderHandler::QuadEncoderHandler(const QuadEncoderSetup &config,
                                       Print &output)
    : config(config), output(output), enc(0) {}
/**
 * Begin the encoder.
 */
void QuadEncoderHandler::Begin() {
  if (!pinCheck(config.kENCA, validEncoderPins, numValidEncoderPins) ||
      !pinCheck(config.kENCB, validEncoderPins, numValidEncoderPins)) {
    output.println("Invalid quad encoder pins");
    return;
  }
  if (encoderNum > 4) {
    output.println("Quad encoder limit reached");
    return;
  }
  encoder =
      std::make_unique<QuadEncoder>(encoderNum, config.kENCA, config.kENCB);
  encoderNum++;
  encoder->setInitConfig();
  encoder->EncConfig.decoderWorkMode = config.workMode;
  encoder->init();
}

/**
 * Update the encoder.
 */
void QuadEncoderHandler::UpdateEnc() {
  if (!encoder) return;
  enc = encoder->read();
  if (config.rev) {
    enc = -enc;
  }
}

/**
 * Return the encoder value.
 */
long QuadEncoderHandler::GetEnc() { return encoder ? enc : 0; }

/**
 * Print the encoder info
 * @param output pass Serial in here.
 * @param printConfig true or false.
 */
void QuadEncoderHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("QuadEncoderHandler Configuration: "));
    output.print(F("kENCA: "));
    // Use if/else here because we print a string or an int
    if (config.kENCA == -1)
      output.print(F("Not Set"));
    else
      output.print(config.kENCA);
    output.print(F(", kENCB: "));
    // Use if/else here because we print a string or an int
    if (config.kENCB == -1)
      output.print(F("Not Set"));
    else
      output.print(config.kENCB);
    output.print(F(", Work Mode: "));
    output.print(config.workMode);
    output.print(F(", Reverse: "));
    output.println(config.rev ? F("True") : F("False"));
  } else {
    output.print(F("QuadEncoderHandler State: "));
    output.print(F("Initialized: "));
    output.print(encoder ? F("True") : F("False"));
    if (encoder) {
      output.print(F(", Current Count: "));
      output.println(enc);
    } else {
      output.println();  // Add a newline even if count isn't printed
    }
  }
}

Print &operator<<(Print &output, const QuadEncoderHandler &encoder) {
  encoder.PrintInfo(output, false);
  return output;
}