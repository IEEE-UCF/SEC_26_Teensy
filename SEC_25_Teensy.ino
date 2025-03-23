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
using namespace GlobalColors;

#include "src/drive/math/Pose2D.h"
#include "src/drive/VectorRobotDrive.h"
#include "src/drive/SimpleRobotDrive.h"
#include "src/drive/VectorRobotDrivePID.h"
#include "src/drive/DriveMotor.h"

extern "C" void startup_early_hook(void);
extern "C" void startup_middle_hook(void);
extern "C" void unused_interrupt_vector(void); // startup.c

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
/*MotorSetup driveMotors[DRIVEMOTOR_COUNT] = {
    {10, 24, 3, 4, true},  // left
    {12, 11, 5, 6, false}, // center
    {25, 9, 7, 8, false}   // right
};

MotorSetup nonDriveMotors[NONDRIVEMOTOR_COUNT] = {
    {28, 29, 31, 30, true}, // intake
    {33, 32, -1, -1, true}  // transfer
};*/

MotorSetup driveMotors[DRIVEMOTOR_COUNT] = {
    {10, 24, 3, 4, true}, // left
    {12, 11, 5, 2, true}, // center
    {25, 9, 7, 8, false}  // right
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
PIDConfig pidConfigs[DRIVEMOTOR_COUNT]{
    {.kp = 1.00f,
     .ki = 0.00f,
     .kd = 0.00f,
     .kaw = 0.00f,
     .timeConst = 0.0f,
     .max = MAX_VELOCITY * 1.2,
     .min = -MAX_VELOCITY * 1.2,
     .maxRate = MAX_ACCELERATION * 1.2,
     .thetaFix = false},
    {.kp = 1.25f,
     .ki = 0.1f,
     .kd = 0.30f,
     .kaw = 0.01f,
     .timeConst = 1.0f,
     .max = MAX_VELOCITY,
     .min = -MAX_VELOCITY,
     .maxRate = MAX_ACCELERATION,
     .thetaFix = false},
    {.kp = 0.0f,
     .ki = 0.000f,
     .kd = 0.000f,
     .kaw = 0.000f,
     .timeConst = 0.0f,
     .max = MAX_ANGULAR_VELOCITY,
     .min = -MAX_ANGULAR_VELOCITY,
     .maxRate = MAX_ANGULAR_ACCELERATION,
     .thetaFix = true},
};
VectorRobotDrivePID drive(driveMotors, DRIVEMOTOR_COUNT, Serial, pidConfigs[0], pidConfigs[0], pidConfigs[2]);
DriveMotor intakeMotor(nonDriveMotors[0], Serial);
DriveMotor transferMotor(nonDriveMotors[1], Serial);

/*
--- Subsystems ---
*/
SorterSubsystem sorter(4, 3, 2, tofs, halls, servos, transferMotor, rgb);
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
  ARMED,
  RUNNING,
  STOPPED,
  ERROR
};

State state;

bool SKIP_LIGHT_SENSOR;
bool CLOSED_POS;
bool CONTROLLED_BY_PI;
bool RESET_AVAILABLE;
bool PI_READY;
bool RC_READY;

bool *dips;

bool SuccessTOF = false;
bool SuccessLight = false;

bool detectLight = false;
/*
--- Statistics ---
*/
long fps = 0;

void setup()
{
  // --- BEGIN SETUP PHASE ---
  state = SETUP;
  buttons.Begin();
  updateDips();
  Serial.begin(9600);
  if (CONTROLLED_BY_PI)
  {
    while (!Serial)
    {
      delay(1);
      if (millis() > 10000)
      {
        reset();
      }
    }
  }
  Serial.println("Hello");

  /* Enable level converters
    pinMode(20, OUTPUT);
    digitalWrite(20, HIGH);
  */

  CrashReport.breadcrumb(2, 00000001);

  // Begin i2c
  Wire.setClock(400000);
  Wire1.setClock(1000000);
  Wire.begin();
  Wire1.begin();

  // Begin handlers
  SuccessTOF = tofs.Begin();
  if (!gyro.Begin())
  {
    reset();
  }
  SuccessLight = light.Begin();
  halls.Begin();
  // buttons.Begin();
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
  updateDips();
  light.Update();
  buttons.Update();
  state = WAITINGFORSTART;
  CrashReport.breadcrumb(2, 00000003);
  rgb.setGlobalBrightness(255);
}

void loop()
{
  switch (state)
  {
  case SETUP:
  {
    delay(10);
    break;
  }
  case WAITINGFORSTART:
  {
    GlobalRead();
    GlobalUpdate();
    GlobalPrint();
    GlobalStats();

    static elapsedMillis update = 0;
    if (update > 50)
    {
      update = 0;
      RC_READY = (rc.Get(9) == 255);
      updateDips(); // updates dips/buttons
      rgb.setSectionSolidColor(2, GOLD);

      // Indicator for Light and TOF I2C
      if (SuccessLight && SuccessTOF)
      {
        rgb.setSectionSolidColor(5, GOLD);
        rgb.setSectionSolidColor(6, GOLD);
      }
      else
      {
        rgb.setSectionPulseEffect(5, GOLD, 20);
        rgb.setSectionPulseEffect(6, GOLD, 20);
      }

      // Setup for servos
      if (CLOSED_POS)
      { // move servos to closed position
        sorter.MoveCenter();
        mandibles.CloseLeft();
        mandibles.CloseRight();
        beacon.MoveUp();
        rgb.setSectionStreakEffect(1, GOLD, 150); // gold
        rgb.setSectionStreakEffect(3, GOLD, 150);
      }
      else
      { // move servos to open position
        sorter.MoveCenter();
        mandibles.OpenLeft();
        mandibles.OpenRight();
        beacon.MoveDown();
        rgb.setSectionStreakEffect(1, PURPLE, 150); // purple
        rgb.setSectionStreakEffect(3, PURPLE, 150);
      }

      // Ready functions for PI/RC
      if (CONTROLLED_BY_PI)
      {
      }
      else
      {
        if (RC_READY)
        {
          rgb.setSectionSolidColor(0, 0, 255, 255);
          rgb.setSectionSolidColor(4, 0, 255, 255);
          if (buttons.GetStates()[0])
          {
            state = ARMED;
            // This logic ensures that skip_light_sensor stays the correct value.
            while (buttons.GetStates()[0])
            {
              updateDips(); // updateDips will also update buttons.
            }
          }
        }
        else
        {
          rgb.setSectionPulseEffect(0, 0, 255, 255, 20);
          rgb.setSectionPulseEffect(4, 0, 255, 255, 20);
        }
      }
    }
    break;
  }
  case ARMED:
  {
    GlobalRead();
    GlobalUpdate();
    GlobalPrint();
    GlobalStats();
    for (uint8_t i = 0; i < 7; i++)
    {
      rgb.setSectionPulseEffect(i, GOLD, 20);
    }
    if (SKIP_LIGHT_SENSOR || light.GetLightLevel() > 500 || buttons.GetStates()[0])
    {
      detectLight = true;
    }
    if (detectLight)
    {
      rgb.stopAllEffects();
      static elapsedMillis timer = 0;
      while (timer < 1000)
      {
        rgb.setSectionStreakEffect(0, GREEN, 66);
        rgb.setSectionStreakEffect(1, GREEN, 100);
        rgb.setSectionStreakEffect(2, GREEN, 100);
        rgb.setSectionStreakEffect(3, GREEN, 100);
        rgb.setSectionStreakEffect(4, GREEN, 66);
        rgb.setSectionStreakEffect(5, GREEN, 250);
        rgb.setSectionStreakEffect(6, GREEN, 250);
        rgb.stopSectionEffect(5);
        rgb.stopSectionEffect(6);
        rgb.setSectionPulseEffect(5, PURPLE, 20);
        rgb.setSectionPulseEffect(6, PURPLE, 20);
        rgb.Update();
      }
      buttons.Update();
      RESET_AVAILABLE = !dips[0];
      state = RUNNING;
    }
    break;
  }
  case RUNNING:
  {
    GlobalRead();
    GlobalUpdate();
    if (GlobalPrint())
    {
      drive.PrintController(Serial, false);
    }
    GlobalStats();

    static elapsedMillis update10hz = 0;
    if (update10hz > 100)
    {
      update10hz = 0;
      if (buttons.GetStates()[0] && RESET_AVAILABLE)
      {
        reset();
      }
      rgb.setSectionSolidColor(0, GOLD);
      rgb.setSectionSolidColor(1, GOLD);
      rgb.setSectionSolidColor(2, GOLD);
      rgb.setSectionSolidColor(3, GOLD);
      rgb.setSectionSolidColor(4, GOLD);
    }

    static elapsedMillis update200hz = 0;
    if (update200hz > 5)
    {
      update200hz = 0;
      sorter.Update();
    }

    drive.ReadAll(gyro.GetGyroData()[0]);
    // Processing
    static elapsedMillis driveUpdate = 0;
    if (driveUpdate > 5)
    {
      driveUpdate = 0;

      Pose2D speedPose = CalculateRCVector(true); // drive.ConstrainNewSpeedPose(CalculateRCVector(true));
      //Serial << speedPose;
      drive.SetTargetByVelocity(speedPose);
      drive.Set(drive.Step());

      /*Pose2D speedPose = drive.ConstrainNewSpeedPose(CalculateRCVector(false));
      drive.Set(speedPose);*/

      int intakeSpeed = rc.Get(5);
      if (abs(intakeSpeed) < 30)
      {
        intakeSpeed = 0;
      }
      intakeMotor.Set(intakeSpeed);

      (rc.Get(6) == -255) ? mandibles.CloseLeft() : mandibles.OpenLeft();
      (rc.Get(7) == -255) ? mandibles.CloseRight() : mandibles.OpenRight();
      if (rc.Get(8) == -255)
      {
        sorter.SetState(1);
      }
      beacon.WriteAngle(map(rc.Get(4), -255, 255, 0, 180));
    }

    // Write
    drive.Write();
    transferMotor.Write();
    intakeMotor.Write();
    break;
  }
  }
}

void GlobalRead()
{
  static elapsedMillis read10hz = 0;
  if (read10hz > 100)
  {
    read10hz = 0;
    light.Update();
    halls.Update();
    buttons.Update();
  }

  static elapsedMillis read20hz = 0;
  if (read20hz > 50)
  {
    read20hz = 0;
    tofs.Update();
  }

  static elapsedMillis read100hz = 0;
  if (read100hz > 10)
  {
    read100hz = 0;
    gyro.Update();
  }

  static elapsedMillis read200hz = 0;
  if (read200hz > 5)
  {
    read200hz = 0;
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
    Serial << tofs << gyro << light << halls << buttons << rgb << servos << rc;
    Serial.print("ControllerPose: ");
    Serial << CalculateRCVector(true) << drive;
    Serial.print("SpeedPose: ");
    Serial << drive.GetVelocity();
    Serial.print("IdealSpeedPose: ");
    Serial << drive.GetIdealVelocity();
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

Pose2D CalculateRCVector(bool positionControl)
{
  float x = map((float)constrain(rc.Get(1), -255, 255), -255, 255, -1, 1);     // RPot Y (x direction at 0 deg)
  float y = map((float)constrain(rc.Get(0), -255, 255), -255, 255, 1, -1);     // RPot X, rev (y direction at 0 deg)
  float theta = map((float)constrain(rc.Get(3), -255, 255), -255, 255, -1, 1); // LPot X
  float yaw = gyro.GetGyroData()[0];
  return drive.CalculateRCVector(x, y, theta, yaw, positionControl);
}

void reset()
{
  startup_middle_hook();
  WRITE_RESTART(0x05FA0004);
  // unused_interrupt_vector();
  servos.Detach();
}

FLASHMEM void startup_middle_hook(void)
{
  for (uint8_t i = 0; i < CORE_NUM_DIGITAL; i++)
  {
    pinMode(i, INPUT_DISABLE);
  }
}

void updateDips()
{
  buttons.Update();
  dips = buttons.GetStates();
  SKIP_LIGHT_SENSOR = dips[0];
  CLOSED_POS = !dips[2];
  CONTROLLED_BY_PI = dips[3];
}