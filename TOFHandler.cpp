#include "TOFHandler.h"

TOFHandler::TOFHandler(int *kPins, int numPins)
{
    this->kPins = kPins;
    this->numPins = numPins;
    sensors = new VL53L0X[numPins];
    distances = new int[numPins];
}

TOFHandler::~TOFHandler()
{
    delete[] sensors;
    delete[] distances;
}

void TOFHandler::Begin()
{
    for (int i = 0; i < numPins; i++)
    {
        pinMode(kPins[i], OUTPUT);
        digitalWrite(kPins[i], LOW);
    }
    delay(10);
    for (int i = 0; i < numPins; i++)
    {
        digitalWrite(kPins[i], HIGH);
        delay(10);
        sensors[i].setTimeout(500);
        if (!sensors[i].init())
        {
            Serial.print(F("Failed to detect and initialize sensor at pin "));
            Serial.println(kPins[i]);
        }
        sensors[i].startContinuous();
    }
}

void TOFHandler::Read()
{
    for (int i = 0; i < numPins; i++)
    {
        distances[i] = sensors[i].readRangeContinuousMillimeters();
        if (sensors[i].timeoutOccurred())
        {
            Serial.print(F("Timeout occurred at pin "));
            Serial.println(kPins[i]);
        }
    }
}

int TOFHandler::Get(int index) const
{
    if (index >= 0 && index < numPins)
    {
        return distances[index];
    }
    return -1; // Return -1 if index is out of bounds
}

void TOFHandler::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("TOFHandler Configuration: "));
    output.print(F("Number of Pins: "));
    output.println(numPins);
    output.print(F("Pins: "));
    for (int i = 0; i < numPins; i++) {
      output.print(kPins[i]);
      if (i < numPins - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  } else {
    output.print(F("Distances: "));
    for (int i = 0; i < numPins; i++) {
      output.print(distances[i]);
      if (i < numPins - 1) {
        output.print(F(", "));
      }
    }
    output.println();
  }
}

Print &operator<<(Print &output, const TOFHandler &handler) {
  handler.PrintInfo(output, false);
  return output;
}