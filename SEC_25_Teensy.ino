#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "src/handler/GyroHandler.h"
#include "src/handler/LineHandler.h"
#include "src/handler/ButtonHandler.h"
// #include "ServoHandler.h"
#include "src/handler/HallHandler.h"
#include "src/handler/RCHandler.h"
// #include "TOFHandler.h"

#include "src/drive/math/Pose2D.h"
#include "src/drive/math/NormalizedPose2D.h"

#include "src/drive/VectorRobotDrive.h"
#include "src/drive/SimpleRobotDrive.h"

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
RCHandler rc;

// LineHandler lines(kLine, LINE_COUNT);

// Output Handlers
VectorRobotDrive robotDrive(kPWM, kCW, kENC, kENCDIR, rev, DRIVEMOTOR_COUNT);
// ServoHandler servos(kServo, SERVO_COUNT);
DriveMotor intake(nkPWM[0], nkCW[0], nkENC[0], nkENCDIR[0], nrev[0]);
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
  rc.Begin(Serial1);
  gyro.Setup();
  // lines.Setup();
  // servos.Setup();
  robotDrive.Begin();
  intake.Begin();

  // Set everything to 0
  robotDrive.Set(NormalizedPose2D(0, 0, 0));
  intake.Set(0);
  robotDrive.Write();
  intake.Write();

  robotDrive.PrintInfo(Serial, true);
  // servos.PrintInfo(Serial, true);
  intake.PrintInfo(Serial, true);
  // sorterMotor.PrintInfo(Serial, true);
  static long temp = millis();
  while (millis() - temp < 3000)
  {
    delay(100);
  }
}

/*
0 - startup
1 - running, controlled by Pi
2 - running, controlled by RC
*/

void loop()
{

  // Read/Update
  gyro.Read();
  rc.Read();
  robotDrive.ReadAll();
  intake.ReadEnc();

  // State Logic
  static int programState = 0;
  if (rc.Get(6) == 255)
  {
    programState = 1; // Down, turn RC off
  }
  else if (rc.Get(6) == -255)
  {
    programState = 2; // Up, turn RC on
  }

  // Main logic
  if (programState == 1)
  {
    NormalizedPose2D hi(0, 0, 0);
    robotDrive.Set(hi);
    intake.Set(0);
  }
  else if (programState == 2)
  {
    int y = rc.Get(2);     // LPot Y
    int x = rc.Get(3);     // LPot X
    int theta = rc.Get(0); // RPot X
    float angleOffset = -gyro.GetGyroData()[2];
    robotDrive.Set(NormalizedPose2D(x, y, theta).rotateVector(angleOffset));
    intake.Set(rc.Get(5));
  }

  intake.Write();
  robotDrive.Write();

  // Debug
  static elapsedMillis printTimer = 0;
  if (printTimer >= 100)
  {
    Serial.print("State: ");
    Serial.println(programState);

    printTimer -= 100;
    // Print Info
    Serial << robotDrive << intake
    /*
    robotDrive.PrintInfo(Serial, false);
    robotDrive.PrintLocal(Serial);
    intake.PrintInfo(Serial, false);*/
    // sorterMotor.PrintInfo(Serial, false);
    // servos.PrintInfo(Serial, false);
    // halls.PrintInfo(Serial, false);
    // tofs.PrintInfo(Serial, false);
    // gyro.PrintInfo(Serial, false);
    // lines.PrintInfo(Serial, false);
  }
}
