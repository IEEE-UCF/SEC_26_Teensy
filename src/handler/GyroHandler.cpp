#include "GyroHandler.h"

GyroHandler::GyroHandler() : bno08x(Adafruit_BNO08x(-1)), Gametime_Offset(0) {}

/**
 * Setup function for the Gyro. Fails sometimes
 */
bool GyroHandler::Begin()
{
    if (!bno08x.begin_I2C(0x4B))
    {
        // if (!bno08x.begin_UART(&Serial1)) {  // Requires a device with > 300 byte
        // UART buffer! if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) {
        Serial.println("Failed to find BNO08x chip");
        return false;
    }
    Serial.println("BNO08x Found!");
    if (!bno08x.enableReport(SH2_GAME_ROTATION_VECTOR))
    {
        Serial.println(F("Could not enable rotation vector"));
        return false;
    }
    return true;
}

/**
 * Reads the gyro data
 */
void GyroHandler::Update()
{
    if (!bno08x.getSensorEvent(&sensorValue))
    {
        return;
    }
    float qr = sensorValue.un.rotationVector.real;
    float qi = sensorValue.un.rotationVector.i;
    float qj = sensorValue.un.rotationVector.j;
    float qk = sensorValue.un.rotationVector.k;

    float sqr = sq(qr);
    float sqi = sq(qi);
    float sqj = sq(qj);
    float sqk = sq(qk);

    float yaw = atan2(2.0 * (qi * qj + qk * qr), (sqi - sqj - sqk + sqr));
    float pitch = asin(-2.0 * (qi * qk - qj * qr) / (sqi + sqj + sqk + sqr));
    float roll = atan2(2.0 * (qj * qk + qi * qr), (-sqi - sqj + sqk + sqr));

    /*
     */
    yaw += (BEGIN_OFFSET * PI / 180) - Gametime_Offset;
    while (yaw > PI)
    {
        yaw = yaw - 2 * PI;
    }
    while (yaw < -PI)
    {
        yaw = yaw + 2 * PI;
    }
    gyroData[0] = yaw;
    gyroData[1] = pitch;
    gyroData[2] = roll;
}

/**
 * Returns the gyro data
 *
 * @return float array, {x, y, z}
 */
float *GyroHandler::GetGyroData()
{
    return gyroData;
}

void GyroHandler::PrintInfo(Print &output, bool printConfig) const
{
    if (printConfig)
    {
        output.println(F("BNO08x Configuration:"));
        output.println(F("I2C Address: 0x55"));
    }
    else
    {
        output.println(F("Gyro Data:"));
        output.print(F("Yaw: "));
        output.println(gyroData[0]);
        output.print(F("Pitch: "));
        output.println(gyroData[1]);
        output.print(F("Roll: "));
        output.println(gyroData[2]);
    }
}
Print &operator<<(Print &output, const GyroHandler &handler)
{
    handler.PrintInfo(output, false);
    return output;
}
