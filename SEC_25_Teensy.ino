

// Include Libraries
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#include "src/handler/ButtonHandler.h"
#include "src/handler/GyroHandler.h"
#include "src/handler/LightHandler.h"
#include "src/handler/HallHandler.h"
#include "src/handler/LineHandler.h"
#include "src/handler/RCHandler.h"
#include "src/handler/RGBHandler.h"
#include "src/handler/ROSHandler.h"
#include "src/handler/ServoHandler.h"
#include "src/handler/TOFHandler.h"
#include "src/subsystem/SorterSubsystem.h"
#include "src/subsystem/MandibleSubsystem.h"
#include "src/subsystem/BeaconSubsystem.h"

#include "src/drive/math/Pose2D.h"

#include "src/drive/VectorRobotDrive.h"
#include "src/drive/SimpleRobotDrive.h"
#include "src/drive/DriveMotor.h"

/*
--- Board Setup ---
*/

// Counts
#define DRIVEMOTOR_COUNT 3
#define NONDRIVEMOTOR_COUNT 2
#define SERVO_COUNT 4
#define TOF_COUNT 5
#define HALL_COUNT 3
#define BUTTON_COUNT 4

// Motor pins
/**
 * kPwm, kCW, kENCA, kENCB, rev
 */
MotorSetup driveMotors[DRIVEMOTOR_COUNT] = {
    {10, 24, 3, 4, true},  // left
    {12, 11, 5, 6, false}, // center
    {25, 9, 7, 8, false}   // right
};

MotorSetup nonDriveMotors[NONDRIVEMOTOR_COUNT] = {
    {28, 29, 31, 30, false}, // intake
    {33, 32, -1, -1, false}  // transfer
};

/**
 * Servo pins: 23, 22, 0, 1, 2
 * TOF channels: 1-7
 * Hall pins: 41, 15, 21, 20
 * Button/Dip pins: 40, 39, 38, 37
 * LED pin: 14
 * RC Rx: Serial8
 * BNO I2C: SDA/SCL
 * TOF/Light I2C: SDA1/SCL1
 */
int kServo[SERVO_COUNT] = {23, 22, 0, 1};
int cTOF[TOF_COUNT] = {1, 2, 3, 4, 5};
int kHall[HALL_COUNT] = {41, 15, 21};
int kButton[BUTTON_COUNT] = {40, 39, 38, 37};
int kLED = 14;
int cLight = 0;

/*
--- Handlers ---
*/
TOFHandler tofs(cTOF, TOF_COUNT);
GyroHandler gyro;
LightHandler light(cLight);
HallHandler halls(kHall, HALL_COUNT);
ButtonHandler buttons(kButton, BUTTON_COUNT);
RGBHandler rgb(kLED);
ServoHandler servos(kServo, SERVO_COUNT);

/*
--- Motors ---
*/
// VectorRobotDrive drive(driveMotors, DRIVEMOTOR_COUNT, Serial);
// DriveMotor intakeMotor(nonDriveMotors[0], Serial);
DriveMotor transferMotor(nonDriveMotors[1], Serial);

/*
--- Subsystems ---
*/
SorterSubsystem sorter(4, 3, 2, tofs, halls, servos, transferMotor);
MandibleSubsystem mandibles(0, 1, servos);
BeaconSubsystem beacon(3, servos);

/*
--- Program Control ---
  DIP Switches/Buttons:
  0 - Start / Wait for light sensor
  1 - X / X
  2 - Closed Pos / Open Pos
  3 - Pi / RC Control
*/

enum State : uint8_t
{
  SETUP,
  WAITINGFORSTART,
  RUNNING,
  STOPPED,
  ERROR
};

State state;

bool SKIP_LIGHT_SENSOR;
bool CLOSED_POS;
bool CONTROLLED_BY_PI;

void setup()
{
  // --- BEGIN SETUP PHASE ---
  state = SETUP;
  Serial.begin(9600);
  if (CrashReport)
  {
    while (!Serial && millis() < 10000)
      ; // wait up to 10sec
    Serial.print(CrashReport);
  }
  while (!Serial)
  {
    delay(1);
  }
  Serial.println("Hello");

  // Initialize
  CrashReport.breadcrumb(2, 00000001);
  Wire.setClock(400000);
  Wire1.setClock(1000000);
  Wire.begin();
  Wire1.begin();

  tofs.Begin();
  gyro.Begin();
  light.Begin();
  halls.Begin();
  buttons.Begin();
  rgb.Begin();
  servos.Begin();
  // drive.Begin();
  // intakeMotor.Begin();
  // transferMotor.Begin();
  sorter.Begin();
  mandibles.Begin();
  beacon.Begin();

  // Print setups
  CrashReport.breadcrumb(2, 00000002);
  tofs.PrintInfo(Serial, true);
  gyro.PrintInfo(Serial, true);
  light.PrintInfo(Serial, true);
  halls.PrintInfo(Serial, true);
  buttons.PrintInfo(Serial, true);
  rgb.PrintInfo(Serial, true);
  servos.PrintInfo(Serial, true);
  sorter.PrintInfo(Serial, true);
  // mandibles.PrintInfo(Serial, true); placeholder
  // beacon.PrintInfo(Serial, true); placeholder

  // --- PROGRAM CONTROL ---
  delay(500);
  light.Update();
  buttons.Update();
  state = WAITINGFORSTART;
  CrashReport.breadcrumb(2, 00000003);
}

void loop()
{
  switch (state)
  {

  case WAITINGFORSTART:
  {
    // Read
    static elapsedMillis read = 0;
    if (read > 100)
    {
      read = 0;
      tofs.Update();
      light.Update();
      halls.Update();
      buttons.Update();
    }

    static elapsedMillis fastRead = 0;
    if (fastRead > 20)
    {
      fastRead = 0;
      gyro.Update();
    }

    // Update
    static elapsedMillis fastUpdate = 0;
    if (fastUpdate > 5)
    {
      fastUpdate = 0;
      rgb.Update();
    }

    static elapsedMillis update = 0;
    if (update > 50)
    {
      update = 0;
      bool *dips = buttons.GetStates();
      SKIP_LIGHT_SENSOR = dips[0];
      CLOSED_POS = dips[2];
      CONTROLLED_BY_PI = dips[3];

      static bool detectLight = false;
      if (SKIP_LIGHT_SENSOR)
      {
        detectLight = true; // auto start the robot
        rgb.Update();
      }
      else
      {
        if (light.GetLightLevel() > 500)
        {
          detectLight = true;
        }
        rgb.setSectionSolidColor(2, 225, 180, 0); // gold
        rgb.Update();
      }
      if (detectLight)
      {
        rgb.setSectionSolidColor(2, 0, 255, 0);
        delay(1000);
        state = RUNNING;
      }

      if (CLOSED_POS)
      { // move servos to closed position
        sorter.MoveLeft();
        mandibles.CloseLeft();
        mandibles.CloseRight();
        beacon.MoveDown();
        rgb.setSectionSolidColor(1, 204, 108, 255);
        rgb.setSectionSolidColor(3, 204, 108, 255);
        rgb.Update();
      }
      else
      { // move servos to open position
        sorter.MoveCenter();
        mandibles.OpenLeft();
        mandibles.OpenRight();
        beacon.MoveUp();
        rgb.setSectionSolidColor(1, 255, 180, 0);
        rgb.setSectionSolidColor(3, 255, 180, 0);
        rgb.Update();
      }

      if (CONTROLLED_BY_PI)
      {
      }
      else
      {
      }
    }

    static elapsedMillis printTimer = 0;
    if (printTimer > 100)
    {
      printTimer = 0;
      printfunc(Serial);
    }

    break;
  }
  case RUNNING:
  {
    // Read
    static elapsedMillis read = 0;
    if (read > 100)
    {
      read = 0;
      tofs.Update();
      light.Update();
      halls.Update();
      buttons.Update();
    }

    static elapsedMillis fastRead = 0;
    if (fastRead > 20)
    {
      fastRead = 0;
      gyro.Update();
    }

    // Update
    static elapsedMillis update = 0;
    if (update > 50)
    {
      update = 0;
    }

    static elapsedMillis fastUpdate = 0;
    if (fastUpdate > 5)
    {
      fastUpdate = 0;
      rgb.Update();
    }

    // Print
    static elapsedMillis printTimer = 0;
    if (printTimer > 100)
    {
      printTimer = 0;
      printfunc(Serial);
    }

    // Statistics
    static elapsedMillis fpsTimer = 0;
    static long cycles = 0;
    cycles++;
    if (fpsTimer >= 1000)
    {
      Serial.print("Cycles: ");
      Serial.println(cycles);
      fpsTimer = 0;
      cycles = 0;
    }
    break;
  }
  }
}

void printfunc(Print &output)
{
  Serial.print("State: ");
  Serial.println(state);
  Serial << tofs << gyro << light << halls << buttons << rgb << servos;
  Serial.println();
}