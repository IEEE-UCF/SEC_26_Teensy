#ifndef TOFHANDLER_H
#define TOFHANDLER_H

#include <Arduino.h>
#include <Print.h>
#include <Wire.h>
#include <VL53L0X.h>

class TOFHandler
{
public:
    TOFHandler(int *kPins, int numPins);
    ~TOFHandler();
    void Begin();
    void Update();
    int Get(int index) const;
    void PrintInfo(Print &output, bool printConfig) const;
    friend Print &operator<<(Print &output, const TOFHandler &handler);

private:
    int *kPins;
    int numPins;
    VL53L0X *sensors;
    int *distances;
};

#endif