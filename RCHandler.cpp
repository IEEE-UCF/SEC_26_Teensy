#include "RCHandler.h"

RCHandler::RCHandler() : updateTimer(0) {
    for (int i = 0; i < 10; i++) {
        pots[i] = 0;
    }
}

void RCHandler::Begin(HardwareSerial &serial)
{
    IBus.begin(serial, IBUSBM_NOTIMER);
}

void RCHandler::Read()
{
    if (updateTimer >= DELAY)
    {
        updateTimer -= DELAY;
        IBus.loop();
        for (int i = 0; i < 10; i++)
        {
            pots[i] = map(constrain(IBus.readChannel(i), 1000, 2000) - 1000, 0, 1000, -255, 255);
        }
    }
}

int32_t RCHandler::Get(int channel)
{
    if (channel >= 0 && channel < 10)
    {
        return pots[channel];
    }
    else
    {
        return 0;
    }
}