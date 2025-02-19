#include "LineHandler.h"

LineHandler::LineHandler(int *kPins, int numPins) {
    this->kPins = kPins;
    this->numPins = numPins;
    lineValues = new int[numPins];
}

LineHandler::~LineHandler() {
    delete[] lineValues;
}

void LineHandler::Setup() {
    for (int i = 0; i < numPins; i++) {
        pinMode(kPins[i], INPUT);
    }
}

void LineHandler::Read() {
    for (int i = 0; i < numPins; i++) {
        lineValues[i] = analogRead(kPins[i]);
    }
}

void LineHandler::PrintInfo(Print &output, bool printConfig) const {
    if (printConfig) {
        output.print(F("LineHandler Configuration: "));
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
        output.print(F("Line Values: "));
        for (int i = 0; i < numPins; i++) {
            output.print(lineValues[i]);
            if (i < numPins - 1) {
                output.print(F(", "));
            }
        }
        output.println();
    }
}

Print &operator<<(Print &output, const LineHandler &handler) {
    handler.PrintInfo(output, false);
    return output;
}