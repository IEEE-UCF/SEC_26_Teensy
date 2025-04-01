/*
Aldem Pido - 4/1/25
LineHandler.h - handles line sensors on a robot
*/
#ifndef LINEHANDLER_H
#define LINEHANDLER_H

#include <Arduino.h>

class LineHandler
{
public:
    LineHandler(int *kPins, int numPins);
    ~LineHandler();
    void Setup();
    void Update();
    void PrintInfo(Print &output, bool printConfig = false) const;

private:
    int *kPins;
    int numPins;
    int *lineValues;
};

// Overload the << operator for Print class
Print &operator<<(Print &output, const LineHandler &handler);

#endif // LINEHANDLER_H