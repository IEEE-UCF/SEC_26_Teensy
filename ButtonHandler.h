#ifndef ButtonHandler_h
#define ButtonHandler_h

#include <Arduino.h>

class ButtonHandler
{
public:
    ButtonHandler(int *kPins, int numPins);
    void Setup();
    void Read();
    bool *buttonStates;

private:
    int *kPins;
    int numPins;
};

#endif