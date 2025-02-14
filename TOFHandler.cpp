#include "Arduino.h"
#include "Wire.h"
#include "VL53L0X.h"
#include "TOFHandler.h"

TOFHandler::TOFHandler(int k1, int k2, int k3, int k4, int k5)
{
    cTOF[0] = k1;
    cTOF[1] = k2;
    cTOF[2] = k3;
    cTOF[3] = k4;
    cTOF[4] = k5;
}

void TOFHandler::Begin()
{
    if (TWCR == 0)
    {
        Serial.begin(9600);
        Serial.println(F("Starting Wire"));
        Wire.begin();
    }

    // Initialize VL53L0X sensors
    for (uint8_t i = 0; i < 5; i++)
    {
        tcaselect(cTOF[i]);
        if (!lox[i].init())
        {
            Serial.print(F("Failed to boot VL53L0X #"));
            Serial.println(i);
        }
        Serial.print(F("Begin VL53L0X #"));
        Serial.println(i);
        lox[i].setTimeout(500);
        lox[i].setSignalRateLimit(0.10);
        lox[i].setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
        lox[i].setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
        lox[i].setMeasurementTimingBudget(20000);
        lox[i].startContinuous();
    }
}

void TOFHandler::Update()
{
    for (uint8_t i = 0; i < 5; i++)
    {
        tcaselect(cTOF[i]);
        read[i] = lox[i].readRangeContinuousMillimeters();
        if (lox[i].timeoutOccurred())
        {
            Serial.print(F(" TIMEOUT"));
        }
    }
}

int TOFHandler::Get(int i)
{
    return read[i];
}
static void TOFHandler::tcaselect(uint8_t i)
{
    if (i > 7)
        return;
    Wire.beginTransmission(TCAADDR);
    Wire.write(1 << i);
    Wire.endTransmission();
}