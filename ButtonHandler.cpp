#include "ButtonHandler.h"
#include <Arduino.h>

ButtonHandler::ButtonHandler(int *kPins, int numPins)
{
    this->kPins = kPins;
    this->numPins = numPins;
    buttonStates = new bool[numPins];
}

void ButtonHandler::Setup()
{
    for (int i = 0; i < numPins; i++)
    {
        pinMode(kPins[i], INPUT_PULLUP);
    }
}

void ButtonHandler::Read()
{
    for (int i = 0; i < numPins; i++)
    {
        buttonStates[i] = digitalRead(kPins[i]) == LOW;
    }
}