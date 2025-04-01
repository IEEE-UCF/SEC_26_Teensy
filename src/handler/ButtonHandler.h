/*
Aldem Pido - 4/1/25
ButtonHandler.h - gets button input (or switch input)
*/
#ifndef ButtonHandler_h
#define ButtonHandler_h

#include <Arduino.h>

class ButtonHandler
{
public:
  ButtonHandler(int *kPins, int numPins);
  void Begin();
  void Update();
  bool *GetStates() const;

  void PrintInfo(Print &output, bool printConfig) const;
  friend Print &operator<<(Print &output, const ButtonHandler &handler);

private:
  bool *buttonStates;
  int *kPins;
  int numPins;
};

#endif