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
    {.kp = 1.0f,
     .ki = 0.000f,
     .kd = 0.000f,
     .kaw = 0.000f,
     .timeConst = 0.0f,
     .max = MAX_VELOCITY,
     .min = -MAX_VELOCITY,
     .maxRate = MAX_ACCELERATION,
     .thetaFix = false},
    {.kp = 1.0f,
     .ki = 0.000f,
     .kd = 0.000f,
     .kaw = 0.000f,
     .timeConst = 0.0f,
     .max = MAX_VELOCITY,
     .min = -MAX_VELOCITY,
     .maxRate = MAX_ACCELERATION,
     .thetaFix = false},
    {.kp = 1.0f,
     .ki = 0.000f,
     .kd = 0.000f,
     .kaw = 0.000f,
     .timeConst = 0.0f,
     .max = MAX_ANGULAR_VELOCITY,
     .min = -MAX_ANGULAR_VELOCITY,
     .maxRate = MAX_ANGULAR_ACCELERATION,
     .thetaFix = true},
};
VectorRobotDrivePID drive(driveMotors, DRIVEMOTOR_COUNT, Serial, pidConfigs[0], pidConfigs[1], pidConfigs[2]);
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
  light.Update();
  buttons.Update();
  state = WAITINGFORSTART;
  CrashReport.breadcrumb(2, 00000003);
  rgb.setGlobalBrightness(200);
}

void loop()
{
  switch (state)
  {

  case WAITINGFORSTART:
  {
    static bool detectLight = false;
    // Read
    GlobalRead();
    // Update
    GlobalUpdate();
    RC_READY = (rc.Get(9) == 255);

    static elapsedMillis update = 0;
    if (update > 50)
    {
      update = 0;
      updateDips();
      rgb.setSectionSolidColor(2, 255, 180, 0); // gold
      if (SuccessLight && SuccessTOF)
      {
        rgb.setSectionSolidColor(5, 255, 180, 0); // gold
        rgb.setSectionSolidColor(6, 255, 180, 0);
      }
      else
      {
        rgb.setSectionPulseEffect(5, 255, 0, 0, 20); // red
        rgb.setSectionPulseEffect(6, 255, 0, 0, 20);
      }

      if (CLOSED_POS)
      { // move servos to closed position
        sorter.MoveCenter();
        mandibles.CloseLeft();
        mandibles.CloseRight();
        beacon.MoveUp();
        rgb.setSectionStreakEffect(1, 255, 180, 0, 150); // gold
        rgb.setSectionStreakEffect(3, 255, 180, 0, 150);
      }
      else
      { // move servos to open position
        sorter.MoveCenter();
        mandibles.OpenLeft();
        mandibles.OpenRight();
        beacon.MoveDown();
        rgb.setSectionStreakEffect(1, 170, 0, 255, 150); // purple
        rgb.setSectionStreakEffect(3, 170, 0, 255, 150);
      }

      if (CONTROLLED_BY_PI)
      {
      }
      else
      {
        if (RC_READY)
        {
          rgb.setSectionSolidColor(0, 0, 255, 255);
          rgb.setSectionSolidColor(4, 0, 255, 255);
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
          }
        }
        else
        {
          rgb.setSectionPulseEffect(0, 0, 255, 255, 20);
          rgb.setSectionPulseEffect(4, 0, 255, 255, 20);
        }
      }
    }

    if (detectLight)
    {
      rgb.stopAllEffects();
      static elapsedMillis timer = 0;
      while (timer < 1000)
      {
        rgb.setSectionStreakEffect(0, 0, 255, 0, 66);
        rgb.setSectionStreakEffect(1, 0, 255, 0, 100);
        rgb.setSectionStreakEffect(2, 0, 255, 0, 100);
        rgb.setSectionStreakEffect(3, 0, 255, 0, 100);
        rgb.setSectionStreakEffect(4, 0, 255, 0, 66);
        rgb.setSectionStreakEffect(5, 0, 255, 0, 250);
        rgb.setSectionStreakEffect(6, 0, 255, 0, 250);
        rgb.Update();
      }
      rgb.stopSectionEffect(5);
      rgb.stopSectionEffect(6);
      rgb.setSectionPulseEffect(5, 170, 0, 255, 20);
      rgb.setSectionPulseEffect(6, 170, 0, 255, 20);
      buttons.Update();
      RESET_AVAILABLE = !dips[0];
      state = RUNNING;
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
    if (update > 150)
    {
      update = 0;
      if (buttons.GetStates()[0] && RESET_AVAILABLE)
      {
        reset();
      }
      rgb.setSectionSolidColor(0, 255, 180, 0);
      rgb.setSectionSolidColor(1, 255, 180, 0);

      rgb.setSectionSolidColor(2, 255, 180, 0);

      rgb.setSectionSolidColor(3, 255, 180, 0);

      rgb.setSectionSolidColor(4, 255, 180, 0);
    }

    static elapsedMillis fastUpdate = 0;
    if (fastUpdate > 5)
    {
      fastUpdate = 0;
      sorter.Update();
    }

    drive.ReadAll(gyro.GetGyroData()[0]);

    // Processing
    static elapsedMillis driveUpdate = 0;
    if (driveUpdate > 5)
    {
      driveUpdate = 0;

      // Pose2D speedPose = drive.ConstrainNewSpeedPose(CalculateRCVector(false));
      // Serial << speedPose;
      //   drive.SetTargetByVelocity(speedPose);
      //    drive.Set(drive.Step());

      Pose2D speedPose = drive.ConstrainNewSpeedPose(CalculateRCVector(false));
      Serial << speedPose;
      drive.Set(speedPose);

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
  if (fastRead > 10)
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
    Serial << tofs << gyro << light << halls << buttons << rgb << servos << rc;
    Serial.print("ControllerPose: ");
    Serial << CalculateRCVector(true) << drive;
    Serial.print("SpeedPose: ");
    Serial << drive.GetVelocity();
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
  dips = buttons.GetStates();
  SKIP_LIGHT_SENSOR = dips[0];
  CLOSED_POS = !dips[2];
  CONTROLLED_BY_PI = dips[3];
}