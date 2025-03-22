#include "RCHandler.h"

RCHandler::RCHandler() : updateTimer(0)
{
    for (int i = 0; i < 10; i++)
    {
        pots[i] = 0;
    }
}

/**
 * Start the RC communication
 */
void RCHandler::Begin(HardwareSerial &serial)
{
    IBus.begin(serial, IBUSBM_NOTIMER);
}

/**
 * Read the values from the rc
 */
void RCHandler::Update()
{
    if (updateTimer >= DELAY)
    {
        updateTimer -= DELAY;
        IBus.loop();
        int32_t newPots[10];
        for (int i = 0; i < 10; i++)
        {
            newPots[i] = map(constrain(IBus.readChannel(i), 1000, 2000) - 1000, 0, 1000, -255, 255);
        }
        if (newPots[9] == -255)
        {
            pots[0] = 0;
            pots[1] = 0;
            pots[2] = 0;
            pots[3] = 0;
            pots[5] = 0;
        }
        else
        {
            for (int i = 0; i < 10; i++)
            {
                pots[i] = newPots[i];
            }
        }
    }
}

/**
 * Returns channel
 *
 * @return returns channel value, from -255 to 255
 */
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

void RCHandler::PrintInfo(Print &output, bool printConfig) const
{
    if (printConfig)
    {
        // Configuration-specific details
        output.println(F("RCHandler Configuration:"));
        output.print(F("Number of Channels: "));
        output.println(10); // Fixed number of channels in this implementation
    }
    else
    {
        // Print all channel states on one line
        output.print(F("RC Channel States: "));
        for (int i = 0; i < 10; i++)
        {
            output.print(pots[i]);
            if (i < 9)
            {
                output.print(F(", ")); // Separate values with commas
            }
        }
        output.println();
    }
}

Print &operator<<(Print &output, const RCHandler &handler)
{
    handler.PrintInfo(output, false); // Print non-configuration details by default
    return output;
}