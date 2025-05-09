/*
Aldem Pido - 4/1/25
GyroHandler.h - gets readings from a bno08x
*/
#ifndef GYROHANDLER_H
#define GYROHANDLER_H

#include <Adafruit_BNO08x.h>
#include <Wire.h>

#define BEGIN_OFFSET 90  // degrees
class GyroHandler {
 public:
  GyroHandler();
  bool Begin();
  void Update();
  void PrintInfo(Print &output, bool printConfig = false) const;
  void Set_Gametime_Offset(float angleRad) { Gametime_Offset = angleRad - BEGIN_OFFSET * PI / 180; }
  float *GetGyroData();

 private:
  Adafruit_BNO08x bno08x;
  sh2_SensorValue_t sensorValue;
  float gyroData[3];
  float Gametime_Offset;
};

// Overload the << operator for Print class
Print &operator<<(Print &output, const GyroHandler &handler);

#endif  // GYROHANDLER_H