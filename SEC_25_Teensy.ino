#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "GyroHandler.h"
#include "LineHandler.h"
#include "Pose2D.h"
#include "NormalizedPose2D.h"
#include "VectorRobotDrive.h"
#include "SimpleRobotDrive.h"
#include "ButtonHandler.h"
// #include "ServoHandler.h"
#include "HallHandler.h"
#include "RCHandler.h"
// #include "TOFHandler.h"

// Constants
#define DRIVEMOTOR_COUNT 3
#define NONDRIVEMOTOR_COUNT 2
// #define SERVO_COUNT 3
#define BUTTON_COUNT 2
#define HALL_COUNT 2
// #define TOF_COUNT 2
#define LINE_COUNT 3

// Motor Setup
int kPWM[DRIVEMOTOR_COUNT] = {9, 11, 10}; // left, center, right
int kCW[DRIVEMOTOR_COUNT] = {32, 27, 26};
int kENC[DRIVEMOTOR_COUNT] = {7, 5, 3};
int kENCDIR[DRIVEMOTOR_COUNT] = {8, 6, 4};
bool rev[DRIVEMOTOR_COUNT] = {true, false, false};

int nkPWM[NONDRIVEMOTOR_COUNT] = {29, -1};
int nkCW[NONDRIVEMOTOR_COUNT] = {30, -1};
int nkENC[NONDRIVEMOTOR_COUNT] = {31, -1};
int nkENCDIR[NONDRIVEMOTOR_COUNT] = {30, -1};
bool nrev[NONDRIVEMOTOR_COUNT] = {true, false};

// Other Input Pins
// const int kButton[BUTTON_COUNT] = {33, 33};
// const int kHall[HALL_COUNT] = {33, 33};
// const int kTOF[TOF_COUNT] = {33, 33};
// const int kLine[LINE_COUNT] = {33, 33, 33};

// Other Output Pins
// const int kServo[SERVO_COUNT] = {33, 33, 33};

// Input Handlers
// ButtonHandler buttons(kButton, BUTTON_COUNT);
// HallHandler halls(kHall, HALL_COUNT);
// TOFHandler tofs(kTOF, TOF_COUNT);
GyroHandler gyro;
RCHandler rcHandler;

// LineHandler lines(kLine, LINE_COUNT);

// Output Handlers
VectorRobotDrive robotDrive(kPWM, kCW, kENC, kENCDIR, rev, DRIVEMOTOR_COUNT);
// ServoHandler servos(kServo, SERVO_COUNT);
DriveMotor intakeMotor(nkPWM[0], nkCW[0], nkENC[0], nkENCDIR[0], nrev[0]);
// DriveMotor sorterMotor(nkPWM[1], nkCW[1], -1, -1, nrev[1]);

void setup()
{
  delay(500);
  Serial.begin(115200);
  Serial.println();

  // Initialize Handlers
  // buttons.Setup();
  // halls.Setup();
  // tofs.Begin();
  rcHandler.Begin(Serial1);
  gyro.Setup();
  // lines.Setup();
  // servos.Setup();
  robotDrive.Begin();
  intakeMotor.Begin();

  robotDrive.PrintInfo(Serial, true);
  // servos.PrintInfo(Serial, true);
  intakeMotor.PrintInfo(Serial, true);
  // sorterMotor.PrintInfo(Serial, true);
  static long temp = millis();
  while (millis() - temp < 3000)
  {
    delay(100);
  }
}

elapsedMillis printTimer = 0;
int programState = 0;
/*
0 - startup
1 - running, controlled by Pi
2 - running, controlled by RC
*/
void loop()
{

  // Read Inputs
  // buttons.Read();
  // halls.Read();
  // tofs.Read();
  gyro.Read();
  rcHandler.Read();
  robotDrive.ReadEnc();
  intakeMotor.ReadEnc();
  // lines.Read();

  if (rcHandler.Get(6) == 255)
  {
    programState = 1; // Down, turn RC off
  }
  else if (rcHandler.Get(6) == -255)
  {
    programState = 2; // Up, turn RC on
  }

  if (printTimer >= 100)
  {
    Serial.print("State: ");
    Serial.println(programState);

    printTimer -= 100;
    // Print Info
    robotDrive.PrintInfo(Serial, false);
    intakeMotor.PrintInfo(Serial, false);
    // sorterMotor.PrintInfo(Serial, false);
    // servos.PrintInfo(Serial, false);
    // halls.PrintInfo(Serial, false);
    // tofs.PrintInfo(Serial, false);
    // gyro.PrintInfo(Serial, false);
    // lines.PrintInfo(Serial, false);
  }
  if (programState == 1)
  {
    NormalizedPose2D hi(0,0,0);
    robotDrive.Set(hi);
    intakeMotor.Set(0);
    intakeMotor.Write();
    robotDrive.Write();
  }
  else if (programState == 2)
  {
    /*int motorSpeeds[3] = { 0 };
    motorSpeeds[2] = rcHandler.Get(1) - map(constrain(rcHandler.Get(0), 0, 255), 0, 255, 0, rcHandler.Get(1));
    motorSpeeds[0] = rcHandler.Get(1) - map(constrain(rcHandler.Get(0), -255, 0), 0, -255, 0, rcHandler.Get(1));
    motorSpeeds[1] = rcHandler.Get(3);*/
    int y = rcHandler.Get(2);     // LPot Y
    int x = rcHandler.Get(3);     // LPot X
    int theta = rcHandler.Get(0); // RPot X
    robotDrive.Set(NormalizedPose2D(x, y, theta));
    intakeMotor.Set(rcHandler.Get(5));

    intakeMotor.Write();
    robotDrive.Write();
  }
}
