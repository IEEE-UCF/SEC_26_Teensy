#include "GyroHandler.h"

GyroHandler::GyroHandler() : bno08x(Adafruit_BNO08x(55)) {}

void GyroHandler::Setup() {
    if (!bno08x.begin_I2C()) {
        Serial.println(F("Failed to find BNO08x chip"));
        while (1) { delay(10); }
    }
    Serial.println(F("BNO08x Found!"));
    bno08x.enableReport(SH2_RAW_GYROSCOPE);
}

void GyroHandler::Read() {
    if (bno08x.getSensorEvent(&gyroEvent)) {
        // Gyro data is now stored in gyroEvent
    }
}

float* GyroHandler::GetGyroData() const {
    gyroData[0] = gyroEvent.un.rawGyroscope.x;
    gyroData[1] = gyroEvent.un.rawGyroscope.y;
    gyroData[2] = gyroEvent.un.rawGyroscope.z;
    return gyroData;
}

void GyroHandler::PrintInfo(Print &output, bool printConfig) const {
    if (printConfig) {
        output.println(F("BNO08x Configuration:"));
        output.println(F("I2C Address: 0x55"));
    } else {
        output.print(F("Gyroscope: "));
        output.print(gyroEvent.un.rawGyroscope.x);
        output.print(F(", "));
        output.print(gyroEvent.un.rawGyroscope.y);
        output.print(F(", "));
        output.print(gyroEvent.un.rawGyroscope.z);
        output.println();
    }
}

Print &operator<<(Print &output, const GyroHandler &handler) {
    handler.PrintInfo(output, false);
    return output;
}