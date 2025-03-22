#ifndef GYROHANDLER_H
#define GYROHANDLER_H

#include <Wire.h>
#include <Adafruit_BNO08x.h>

#define STARTING_OFFSET 90 // degrees
class GyroHandler
{
public:
    GyroHandler();
    bool Begin();
    void Update();
    void PrintInfo(Print &output, bool printConfig = false) const;
    float *GetGyroData();

private:
    Adafruit_BNO08x bno08x;
    sh2_SensorValue_t sensorValue;
    float gyroData[3];
};

// Overload the << operator for Print class
Print &operator<<(Print &output, const GyroHandler &handler);

#endif // GYROHANDLER_H