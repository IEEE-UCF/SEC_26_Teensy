#ifndef MANDIBLESUBSYSTEM_H
#define MANDIBLESUBSYSTEM_H

#include <Arduino.h>
#include "../handler/ServoHandler.h"

class MandibleSubsystem
{
public:
    MandibleSubsystem(int indexLeft, int indexRight, ServoHandler &servos);
    void Begin();
    // void Update();
    void OpenLeft();
    void CloseLeft();
    void OpenRight();
    void CloseRight();
    void SetState();

    enum Positions : uint8_t
    {
        LEFT_OPEN = 180,
        LEFT_CLOSE = 115,
        RIGHT_OPEN = 0,
        RIGHT_CLOSE = 55
    };

private:
    int indexLeft;
    int indexRight;
    ServoHandler &servos;
};

#endif