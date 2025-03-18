#ifndef BEACONSUBSYSTEM_H
#define BEACONSUBSYSTEM_H

#include <Arduino.h>
#include "../handler/ServoHandler.h"

class BeaconSubsystem
{
public:
    BeaconSubsystem(int indexBeacon, ServoHandler &servos);
    void Begin();
    void MoveUp();
    void MoveDown(int offset = 0);

    enum Positions : uint8_t
    {
        UP = 0,
        DOWN = 65
    };

private:
    int indexBeacon;
    ServoHandler &servos;
};

#endif