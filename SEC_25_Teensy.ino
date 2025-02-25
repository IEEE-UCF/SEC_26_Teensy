#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "GyroHandler.h"
#include "LineHandler.h"
#include "Pose2D.h"
#include "VectorRobotDrive.h"
#include "ButtonHandler.h"
//#include "ServoHandler.h"
#include "HallHandler.h"
//#include "TOFHandler.h"

// Constants
#define DRIVEMOTOR_COUNT 3
#define NONDRIVEMOTOR_COUNT 2
//#define SERVO_COUNT 3
#define BUTTON_COUNT 2
#define HALL_COUNT 2
//#define TOF_COUNT 2
#define LINE_COUNT 3

// Motor Setup
const int kPWM[DRIVEMOTOR_COUNT] = {9, 11, 10};
const int kCW[DRIVEMOTOR_COUNT] = {8, 6, 4};
const int kENC[DRIVEMOTOR_COUNT] = {7, 5, 3};
const bool rev[DRIVEMOTOR_COUNT] = {false, false, true};

const int nkPWM[NONDRIVEMOTOR_COUNT] = {29, 33};
const int nkCW[NONDRIVEMOTOR_COUNT] = {30, 33};
const bool nrev[NONDRIVEMOTOR_COUNT] = {false, false};

// Other Input Pins
//const int kButton[BUTTON_COUNT] = {33, 33};
//const int kHall[HALL_COUNT] = {33, 33};
//const int kTOF[TOF_COUNT] = {33, 33};
//const int kLine[LINE_COUNT] = {33, 33, 33};

// Other Output Pins
//const int kServo[SERVO_COUNT] = {33, 33, 33};

// Input Handlers
//ButtonHandler buttons(kButton, BUTTON_COUNT);
//HallHandler halls(kHall, HALL_COUNT);
//TOFHandler tofs(kTOF, TOF_COUNT);
GyroHandler gyro;
//LineHandler lines(kLine, LINE_COUNT);

// Output Handlers
VectorRobotDrive robotDrive(kPWM, kCW, kENC, rev, DRIVEMOTOR_COUNT);
//ServoHandler servos(kServo, SERVO_COUNT);
DriveMotor intakeMotor(nkPWM[0], nkCW[0], -1, nrev[0]);
DriveMotor sorterMotor(nkPWM[1], nkCW[1], -1, nrev[1]);

void setup() {
    Serial.begin(115200);
    while(!Serial) {
      delay(10);
    }
    Serial.println();

    // Initialize Handlers
    //buttons.Setup();
    //halls.Setup();
    //tofs.Begin();
    gyro.Setup();
    //lines.Setup();
    //servos.Setup();
    intakeMotor.Begin();
    robotDrive.Begin();

    //robotDrive.PrintInfo(Serial, true);
    //servos.PrintInfo(Serial, true);
    intakeMotor.PrintInfo(Serial, true);
    sorterMotor.PrintInfo(Serial, true);
    static long temp = millis();
    while(millis() - temp < 3000) {
      delay(100);
    }
}

void loop() {
    // Read Inputs
    //buttons.Read();
    //halls.Read();
    //tofs.Read();
    gyro.Read();
    //lines.Read();

    // Set Outputs
    int speeds[3] = {255, 255, 255};
    robotDrive.Set(speeds);
    delay(10);
    intakeMotor.Set(-255);
    //sorterMotor.Set(0);

    // Write Outputs
    robotDrive.Write();
    intakeMotor.Write();
    //sorterMotor.Write();

    // Print Info
    robotDrive.PrintInfo(Serial, false);
    intakeMotor.PrintInfo(Serial, false);
    sorterMotor.PrintInfo(Serial, false);
    //servos.PrintInfo(Serial, false);
    //halls.PrintInfo(Serial, false);
    //tofs.PrintInfo(Serial, false);
    gyro.PrintInfo(Serial, false);
    //lines.PrintInfo(Serial, false);

    delay(100);
}

