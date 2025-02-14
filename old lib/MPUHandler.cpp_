#include "Arduino.h"
#include "RGBHandler.h"
#include "Wire.h"
#include "MPU6050_light.h"
#include "MPUHandler.h"
#include "TOFHandler.h"

void MPUHandler::Begin(RGBHandler rgb) {
  if (TWCR == 0) {
    Serial.begin(9600);
    Serial.println(F("Starting Wire"));
    Wire.begin();
  }
  TOFHandler::tcaselect(channel);
  Serial.println(F("MUX set to "));
  Serial.println(channel);
  _status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(_status);

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  rgb.setRGB(1, 0, 0);
  delay(2000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets();  // gyro and accelero
  rgb.clearRGB();
  Serial.println(F("Done!\n"));
}

void MPUHandler::Update() {
  TOFHandler::tcaselect(channel);
  mpu.update();
  if (_status != 0) {
    angles[0] = 0;
    angles[1] = 0;
    angles[2] = 0;
  } else {
    angles[0] = mpu.getAngleX();
    angles[1] = mpu.getAngleY();
    angles[2] = mpu.getAngleZ();
  }
}

int MPUHandler::Get(int dimension) {
  return angles[dimension];
}