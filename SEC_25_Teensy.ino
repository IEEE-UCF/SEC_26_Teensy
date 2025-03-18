

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
    {28, 29, 31, 30, true}, // intake
    {33, 32, -1, -1, true}  // transfer
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
RCHandler rc;

/*
--- Motors ---
*/
VectorRobotDrive drive(driveMotors, DRIVEMOTOR_COUNT, Serial);
DriveMotor intakeMotor(nonDriveMotors[0], Serial);
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

#define RESTART_ADDR 0xE000ED0C
#define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

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
bool RESET_AVAILABLE;

/*
--- Statistics ---
*/
long fps = 0;

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
  if (!gyro.Begin())
    WRITE_RESTART(0x5FA0004);
  light.Begin();
  halls.Begin();
  buttons.Begin();
  rgb.Begin();
  servos.Begin();
  rc.Begin(Serial8);
  drive.Begin();
  intakeMotor.Begin();
  transferMotor.Begin();
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
  rc.PrintInfo(Serial, true);
  drive.PrintInfo(Serial, true);
  Serial.println("Intake: ");
  intakeMotor.PrintInfo(Serial, true);
  Serial.println("Transfer: ");
  transferMotor.PrintInfo(Serial, true);
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
    GlobalRead();
    // Update
    GlobalUpdate();

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
      }
      else
      {
        if (light.GetLightLevel() > 500)
        {
          detectLight = true;
        }
        rgb.setSectionSolidColor(2, 225, 180, 0); // gold
      }
      if (detectLight)
      {
        rgb.setSectionSolidColor(2, 0, 255, 0);
        delay(1000);
        buttons.Update();
        RESET_AVAILABLE = !dips[0];
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
      }
      else
      { // move servos to open position
        sorter.MoveCenter();
        mandibles.OpenLeft();
        mandibles.OpenRight();
        beacon.MoveUp();
        rgb.setSectionSolidColor(1, 255, 180, 0);
        rgb.setSectionSolidColor(3, 255, 180, 0);
      }

      if (CONTROLLED_BY_PI)
      {
      }
      else
      {
        if (rc.Get(9) == 255)
        {
          rgb.setSectionPulseEffect(0, 255, 180, 0, 20);
          rgb.setSectionPulseEffect(4, 255, 180, 0, 20);
        }
        else
        {
        }
      }
    }

    // Print
    GlobalPrint();

    // Stats
    GlobalStats();

    break;
  }
  case RUNNING:
  {
    // Read
    GlobalRead();

    // Update
    GlobalUpdate();

    static elapsedMillis update = 0;
    if (update > 50)
    {
      update = 0;
      if (buttons.GetStates()[0] && RESET_AVAILABLE)
      {
        WRITE_RESTART(0x5FA0004);
      }
    }

    static elapsedMillis fastUpdate = 0;
    if (fastUpdate > 5)
    {
      fastUpdate = 0;
      sorter.Update();
    }

    drive.ReadAll();

    // Processing
    Pose2D toWrite;
    static elapsedMillis driveUpdate = 0;
    if (driveUpdate > 5)
    {
      driveUpdate = 0;
      toWrite = CalculateRCVector();
      drive.Set(toWrite);
      int intakeSpeed = rc.Get(5);
      if (abs(intakeSpeed) < 30)
      {
        intakeSpeed = 0;
      }
      intakeMotor.Set(intakeSpeed);

      (rc.Get(6) == -255) ? mandibles.CloseLeft() : mandibles.OpenLeft();
      (rc.Get(7) == -255) ? mandibles.CloseRight() : mandibles.OpenRight();
      beacon.WriteAngle(map(rc.Get(4), -255, 255, 0, 180));
    }

    // Write
    drive.Write();
    transferMotor.Write();
    intakeMotor.Write();

    // Print
    if (GlobalPrint())
    {
    }

    // Statistics
    GlobalStats();

    break;
  }
  }
}

void GlobalRead()
{
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

  static elapsedMillis lightningRead = 0;
  if (lightningRead > 5)
  {
    lightningRead = 0;
    rc.Update();
  }
}

void GlobalUpdate()
{
  static elapsedMillis rgbUpdate = 0;
  if (rgbUpdate > 20)
  {
    rgbUpdate = 0;
    rgb.Update();
  }
}

bool GlobalPrint()
{
  static elapsedMillis printTimer = 0;
  if (printTimer > 100)
  {
    printTimer = 0;
    Serial.print("State: ");
    Serial.println(state);
    Serial.print("FPS: ");
    Serial.println(fps);
    Serial << tofs << gyro << light << halls << buttons << rgb << servos << rc << CalculateRCVector() << drive;
    Serial.print("Transfer: ");
    Serial << transferMotor;
    Serial.print("Intake: ");
    Serial << intakeMotor;
    return true;
  }
  else
  {
    return false;
  }
}

bool GlobalStats()
{
  static elapsedMillis fpsTimer = 0;
  static long cycles = 0;
  cycles++;
  static long lastCycles = 0;
  if (fpsTimer >= 1000)
  {
    fpsTimer = 0;
    lastCycles = cycles;
    fps = lastCycles;
    cycles = 0;
    return true;
  }
  else
  {
    return false;
  }
}

Pose2D CalculateRCVector()
{
  float y = map((float)constrain(rc.Get(1), -255, 255), -255, 255, -1, 1); // RPot Y
  y *= MotorConstants::MAX_VELOCITY;
  float x = map((float)constrain(rc.Get(0), -255, 255), -255, 255, -1, 1); // RPot X
  x *= MotorConstants::MAX_VELOCITY;
  float theta = map((float)constrain(rc.Get(3), -255, 255), -255, 255, -1, 1); // LPot X
  theta *= MotorConstants::MAX_ANGULAR_VELOCITY;
  float angleOffset = -gyro.GetGyroData()[0];
  Pose2D angleOffsetPose(0, 0, angleOffset);
  angleOffsetPose.fixTheta();
  return Pose2D(x, y, theta).rotateVector(angleOffsetPose.getTheta());
}