/*
Aldem Pido - 4/1/25
LightHandler.h - handles BH1750 light sensor
*/
#ifndef LIGHTHANDLER_H
#define LIGHTHANDLER_H

#include <BH1750.h>
#include <Wire.h>

#include "i2cmux.h"

class LightHandler {
 public:
  LightHandler(int cLight);
  bool Begin();
  void Update();
  void PrintInfo(Print &output, bool printConfig = false) const;
  float GetLightLevel() const;

 private:
  int cLight;
  BH1750 lightMeter;
  float lightLevel;
};

// Overload the << operator for Print class
Print &operator<<(Print &output, const LightHandler &handler);

#endif  // LIGHTHANDLER_H