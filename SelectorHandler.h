#ifndef SelectorHandler_h
#define SelectorHandler_h

#include "Arduino.h"
#include "RGBHandler.h"

class SelectorHandler
{
public:
    SelectorHandler(int k1, int k2);
    int ReadSel();
    int GetSel();
    void IndicateSelection(RGBHandler rgb);

private:
    int kSel[2];
    int sel[2];
    void tcaselect(uint8_t i);
};

#endif