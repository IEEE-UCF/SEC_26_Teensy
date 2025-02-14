#include "SelectorHandler.h"
#include "Arduino.h"
#include "RGBHandler.h"

SelectorHandler::SelectorHandler(int k1, int k2)
{
    kSel[0] = k1;
    kSel[1] = k2;
    for (int i = 0; i < 2; i++)
    {
        pinMode(kSel[i], INPUT_PULLUP);
    }
}
int SelectorHandler::GetSel()
{
    return sel[1] * 2 + sel[0] * 1;
}
int SelectorHandler::ReadSel()
{
    for (int i = 0; i < 2; i++)
    {
        sel[i] = !digitalRead(kSel[i]);
    }
    return sel[1] * 2 + sel[0] * 1;
}

void SelectorHandler::IndicateSelection(RGBHandler rgb)
{
    int x = sel[1] * 2 + sel[0] * 1;
    rgb.clearRGB();
    delay(400);
    for (int i = 0; i < x; i++)
    {
        rgb.setRGB(7);
        delay(400);
        rgb.clearRGB();
        delay(400);
    }
}