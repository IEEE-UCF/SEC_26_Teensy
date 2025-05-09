/*
Aldem Pido, Tino Hernandez - 4/1/25
RCHandler.h - interfaces with flysky receiver
*/
#ifndef RCHANDLER_H
#define RCHANDLER_H

#include <IBusBM.h>
#include <elapsedMillis.h>

#define DELAY 2
class RCHandler {
 public:
  RCHandler();
  void Begin(HardwareSerial &serial);
  void Update();
  int32_t Get(int channel);
  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const RCHandler &handler);

 private:
  IBusBM IBus;
  int32_t pots[10];
  elapsedMillis updateTimer;
  /**
  int32_t pots[10] = {0};
  int32_t pots_old[10] = {0};
  const char *pot_names[10] = {
      "RPot X",
      "RPot Y",
      "LPot Y",
      "LPot X",
      "AuxA",
      "AuxB",
      "Aux1",
      "Aux2",
      "Aux3",
      "Aux4"};*/
};
#endif