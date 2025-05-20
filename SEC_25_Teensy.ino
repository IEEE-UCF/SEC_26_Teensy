/*
Aldem Pido - 4/1/25
SEC_25_Teensy.ino - main code control for the Teensy board
Developed for IEEE Southeastcon Hardware Competition 2025
*/

// Include Libraries
#include <Wire.h>

#include "src/handler/ButtonHandler.h"
#include "src/handler/GyroHandler.h"
#include "src/handler/HallHandler.h"
#include "src/handler/LightHandler.h"
#include "src/handler/LineHandler.h"
#include "src/handler/RCHandler.h"
#include "src/handler/RGBHandler.h"
// #include "src/handler/ROSHandler.h"
#include "src/handler/PathHandler.h"
#include "src/handler/ServoHandler.h"
#include "src/handler/TOFHandler.h"
#include "src/subsystem/BeaconSubsystem.h"
#include "src/subsystem/MandibleSubsystem.h"
#include "src/subsystem/SorterSubsystem.h"
using namespace GlobalColors;

#include "src/drive/DriveMotor.h"
#include "src/drive/SimpleRobotDrive.h"
#include "src/drive/VectorRobotDrive.h"
#include "src/drive/VectorRobotDrivePID.h"
#include "src/drive/math/Pose2D.h"
#include "src/drive/paths.h"

// These functions are used to control the state of pins while the teensy starts.
extern "C" void startup_early_hook(void);
extern "C" void startup_middle_hook(void);
extern "C" void unused_interrupt_vector(void);  // startup.c

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

MotorSetup driveMotors[DRIVEMOTOR_COUNT] = {
    {10, 24, 3, 4, true},  // left
    {12, 11, 5, 2, true},  // center
    {25, 9, 7, 8, false}   // right
};

MotorSetup nonDriveMotors[NONDRIVEMOTOR_COUNT] = {
    {28, 29, 31, 30, true},  // intake
    {33, 32, -1, -1, true}   // transfer
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
    {.kp = 8.00f,
     .ki = 0.00f,
     .kd = 0.05f,
     .kaw = 0.00f,
     .timeConst = 0.5f,
     .max = MAX_VELOCITY,
     .min = -MAX_VELOCITY,
     .maxRate = MAX_ACCELERATION,
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
    {.kp = 7.0f,
     .ki = 0.0f,
     .kd = 0.3f,
     .kaw = 0.00f,
     .timeConst = 0.5f,
     .max = MAX_ANGULAR_VELOCITY,
     .min = -MAX_ANGULAR_VELOCITY,
     .maxRate = MAX_ANGULAR_ACCELERATION,
     .thetaFix = true},
};
VectorRobotDrivePID drive(driveMotors, DRIVEMOTOR_COUNT, Serial, pidConfigs[0], pidConfigs[0],
                          pidConfigs[2]);
DriveMotor intakeMotor(nonDriveMotors[0], Serial);
DriveMotor transferMotor(nonDriveMotors[1], Serial);

/*
--- Subsystems ---
*/
SorterSubsystem sorter(4, 3, 2, tofs, halls, servos, transferMotor, rgb);
MandibleSubsystem mandibles(0, 1, servos);
BeaconSubsystem beacon(3, servos);
PathHandler paths(drive);

/*
--- Program Control ---
  DIP Switches/Buttons:
  0 - Manual start button
  1 - x / x
  2 - External Control / HARD Coded Path
  3 - Pi / RC Control
*/

#define RESTART_ADDR 0xE000ED0C
#define READ_RESTART() (*(volatile uint32_t *)RESTART_ADDR)
#define WRITE_RESTART(val) ((*(volatile uint32_t *)RESTART_ADDR) = (val))

enum State : uint8_t { SETUP, WAITINGFORSTART, ARMED, RUNNING, STOPPED, ERROR };

enum ControlType : uint8_t { HARD, RASP_PI, REMOTE_CON };

enum HardCodeProgram : uint8_t { NO_BOX, BOX };

State STATE;
ControlType ROBOT_CONTROL_TYPE;
HardCodeProgram PROGRAM_SELECTION;

bool USING_EXTERNAL_CONTROL;
bool CONTROLLED_BY_PI;
bool CLOSED_POS;

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

void setup() {
  // --- BEGIN SETUP PHASE ---
  STATE = SETUP;
  buttons.Begin();  // Necessary to begin buttons here to get CONTROLLED_BY_PI
  updateDips();
  Serial.begin(9600);
  /*if (CONTROLLED_BY_PI)
  {
    while (!Serial)
    {
      delay(1);
      if (millis() > 10000)
      {
        reset();
      }
    }
  }*/
  Serial.println("Hello");

  // Begin I2C
  Wire.setClock(400000);
  Wire1.setClock(1000000);
  Wire.begin();
  Wire1.begin();

  // Begin all Handlers
  SuccessTOF = tofs.Begin();
  if (!gyro.Begin()) {
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
  updateDips();  // Update dips so we don't get any unexpected readings
  light.Update();
  buttons.Update();
  STATE = WAITINGFORSTART;
  rgb.setGlobalBrightness(175);
}

void loop() {
  switch (STATE) {
    case SETUP: {
      delay(10);
      break;
    }
    case WAITINGFORSTART: {
      GlobalRead();
      GlobalUpdate();
      GlobalPrint();
      GlobalStats();

      static elapsedMillis update = 0;
      if (update > 50) {
        update = 0;    // update every 50 ms
        updateDips();  // updates dips/buttons

        // --- Indicator for Light and TOF I2C ---
        if (SuccessLight && SuccessTOF) {
          rgb.setSectionSolidColor(5, GOLD);
          rgb.setSectionSolidColor(6, GOLD);
        } else {
          rgb.setSectionPulseEffect(5, RED, 20);
          rgb.setSectionPulseEffect(6, RED, 20);
        }

        rgb.setSectionStreakEffect(1, GOLD, 200);
        rgb.setSectionStreakEffect(3, GOLD, 200);

        // --- Ready function ---
        bool READY_TO_ARM;
        RGBColor sideColor;
        ROBOT_CONTROL_TYPE =
            USING_EXTERNAL_CONTROL ? HARD : (CONTROLLED_BY_PI ? RASP_PI : REMOTE_CON);
        PROGRAM_SELECTION = dips[1] ? BOX : NO_BOX;
        // Determine correct control type
        switch (ROBOT_CONTROL_TYPE) {
          case HARD: {
            sideColor = PURPLE;
            READY_TO_ARM = true;
            switch (PROGRAM_SELECTION) {
              case NO_BOX: {
                rgb.setSectionSolidColor(2, GREEN);
                break;
              }
              case BOX: {
                rgb.setSectionSolidColor(2, CYAN);
                break;
              }
            }
            break;
          }
          case RASP_PI: {
            sideColor = GOLD;
            READY_TO_ARM = false;
            rgb.setSectionSolidColor(2, GOLD);
            break;
          }
          case REMOTE_CON: {
            sideColor = CYAN;
            RC_READY = (rc.Get(9) == 255);
            READY_TO_ARM = RC_READY;
            rgb.setSectionSolidColor(2, GOLD);
            break;
          }
        }

        if (READY_TO_ARM)  // Ready to arm!
        {
          rgb.setSectionSolidColor(0, sideColor);
          rgb.setSectionSolidColor(4, sideColor);
          if (buttons.GetStates()[0]) {
            STATE = ARMED;
            while (buttons.GetStates()[0]) {
              updateDips();  // Wait until button.GetStates() is released
            }
            servos.Attach();
            buttons.Update();
          }
        } else  // Not ready to arm!
        {
          rgb.setSectionPulseEffect(0, sideColor, 20);
          rgb.setSectionPulseEffect(4, sideColor, 20);
        }
      }
      break;
    }
    case ARMED: {
      GlobalRead();
      GlobalUpdate();
      GlobalPrint();
      GlobalStats();
      for (uint8_t i = 0; i < 5; i++) {
        rgb.setSectionPulseEffect(i, GOLD, 20);
      }

      // --- Servo Setup ---
      switch (ROBOT_CONTROL_TYPE) {
        case HARD:
        case RASP_PI:
          CLOSED_POS = true;
          break;
        case REMOTE_CON:
          CLOSED_POS = (rc.Get(8) == 255);
          break;
      }
      if (CLOSED_POS) {  // move servos to closed position
        sorter.MoveCenter();
        mandibles.CloseLeft();
        mandibles.CloseRight();
        beacon.MoveUp();
        rgb.setSectionStreakEffect(5, GOLD, 50);  // gold
        rgb.setSectionStreakEffect(6, GOLD, 50);
      } else {  // move servos to open position
        sorter.MoveCenter();
        mandibles.OpenLeft();
        mandibles.OpenRight();
        beacon.MoveDown();
        rgb.setSectionStreakEffect(5, PURPLE, 150);  // purple
        rgb.setSectionStreakEffect(6, PURPLE, 150);
      }

      if (light.GetLightLevel() > 75 || buttons.GetStates()[0]) {
        rgb.stopAllEffects();
        static elapsedMillis timer = 0;
        while (timer < 1000) {
          rgb.setSectionStreakEffect(0, GREEN, 66);
          rgb.setSectionStreakEffect(1, GREEN, 100);
          rgb.setSectionStreakEffect(2, GREEN, 100);
          rgb.setSectionStreakEffect(3, GREEN, 100);
          rgb.setSectionStreakEffect(4, GREEN, 66);
          rgb.setSectionStreakEffect(5, GREEN, 250);
          rgb.setSectionStreakEffect(6, GREEN, 250);
          rgb.Update();
        }
        gyro.Update();
        gyro.Set_Gametime_Offset(gyro.GetGyroData()[0]);
        updateDips();
        RESET_AVAILABLE = !dips[0];
        rgb.setSectionPulseEffect(5, PURPLE, 20);
        rgb.setSectionPulseEffect(6, PURPLE, 20);
        STATE = RUNNING;
      }
      break;
    }
    case RUNNING: {
      static elapsedMillis update10hz = 0;
      static elapsedMillis update200hz = 0;  // 200hz
      static bool update10Available = false;
      static bool update200Available = false;
      if (update10hz >= 100) {
        update10hz = 0;
        update10Available = true;
      }
      if (update200hz >= 5) {
        update200hz = 0;
        update200Available = true;
      }
      switch (ROBOT_CONTROL_TYPE) {
        case HARD:
        case RASP_PI: {
          GlobalRead();
          GlobalUpdate();
          GlobalStats();
          if (GlobalPrint()) {
            // drive.PrintController(Serial, false);
          }
          drive.ReadAll(gyro.GetGyroData()[0]);
          switch (PROGRAM_SELECTION) {
            case NO_BOX:  // Green
            {
              if (update10Available) {
                update10Available = false;
                if (buttons.GetStates()[0] && RESET_AVAILABLE)  // Reset function
                {
                  reset();
                }
                for (int i = 0; i < 5; i++)  // Write RGB
                {
                  rgb.setSectionSolidColor(i, GOLD);
                }
                break;
              }

              if (update200Available) {
                update200Available = false;
                // Path logic
                static int path_index = -1;
                static bool command_set = false;
                static elapsedMillis command_timer = 0;
                switch (path_index) {
                  case -1:  // start
                    command_set = false;
                    path_index++;
                    break;
                  case 0:  // slam into the left corner
                    if (!command_set) {
                      paths.addWaypoints(Paths::slam_left_corner);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(6, 6, PI * 0.5));
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 1:  // go to beacon position
                    if (!command_set) {
                      paths.addWaypoints(Paths::beacon_position);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 2:  // lower servo arm
                    if (!command_set) {
                      beacon.MoveDown();
                      command_set = true;
                      command_timer = 0;
                    }
                    if (command_timer > 2000) {
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 3:  // jostle beacon
                    if (!command_set) {
                      paths.addWaypoints(Paths::jostle_beacon);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 4:  // slide out beacon
                    if (!command_set) {
                      paths.addWaypoints(Paths::beacon_pullout);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      command_set = false;
                      path_index++;
                    }
                    break;
                }
                /*static bool path_clear = false;
                path_clear = paths.executePath();
                if (path_clear)
                {
                  paths.addWaypoints(Paths::andrew_waypoint);
                }*/
              }

              // Drive update
              drive.Set(drive.Step());
              drive.Write();
              break;
            }
            case BOX:  // Cyan
            {
              if (update10Available) {
                update10Available = false;
                if (buttons.GetStates()[0] && RESET_AVAILABLE)  // Reset function
                {
                  reset();
                }
                for (int i = 0; i < 5; i++)  // Write RGB
                {
                  rgb.setSectionSolidColor(i, GOLD);
                }
                break;
              }
              static int path_index = -1;
              if (update200Available) {
                update200Available = false;
                // Path logic
                static bool command_set = false;
                static elapsedMillis command_timer = 0;
                switch (path_index) {
                  case -1:  // start
                    command_set = false;
                    path_index++;
                    break;
                  case 0:  // start to slam sw north
                    if (!command_set) {
                      paths.addWaypoints(HardBox::startToSlamSW90);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(6, 6, NORTH));
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 1:  // go to beacon position
                    if (!command_set) {
                      paths.addWaypoints(HardBox::supposedBeacon);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      beacon.MoveDown();
                      static elapsedMillis timer = 0;
                      while (timer < 0) {
                      }
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 2:  // jostle beacon, pullout
                    if (!command_set) {
                      paths.addWaypoints(HardBox::jostleBeacon);
                      paths.addWaypoint(Pose2D(BEACONX + 10, BEACONY, NORTH));  // slide beacon out)
                      paths.addWaypoint(Pose2D(BEACONX + 5, MAXY - 3, NORTH));
                      paths.addWaypoint(Pose2D(BEACONX + 5, MAXY - 13, NORTH));
                      paths.addWaypoint(Pose2D(BEACONX - 1, MAXY - 13, NORTH));
                      paths.addWaypoint(Pose2D(BEACONX - 1, MAXY - 3, NORTH));
                      paths.addWaypoint(Pose2D(BEACONX + 5, BEACONY - 4, NORTH));

                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      beacon.MoveUp();
                      mandibles.OpenLeft();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 3:  // go to get csc
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(70, BEACONY - 4, NORTH));
                      paths.addWaypoint(Pose2D(50, BEACONY - 4, NORTH));
                      paths.addWaypoint(Pose2D(37, MAXY - 6, NORTH));
                      paths.addWaypoint(Pose2D(10, MAXY - 6, NORTH));
                      paths.addWaypoint(Pose2D(30, MAXY - 22, NORTH));
                      paths.addWaypoint(Pose2D(30, MAXY - 22, EAST));
                      paths.addWaypoint(Pose2D(30, MAXY - 2, EAST));
                      paths.addWaypoint(Pose2D(2, MAXY - 2, EAST));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index = 7;
                    }
                    break;
                  case 7:  // sweep north
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(12, MAXY - 6, EAST));
                      paths.addWaypoint(Pose2D(50, MAXY - 8, EAST));
                      paths.addWaypoint(Pose2D(12, MAXY - 8, EAST));
                      paths.addWaypoint(Pose2D(18, MAXY - 3, EAST));
                      paths.addWaypoint(Pose2D(10, MAXY - 3, EAST));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 8:
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(12, MAXY - 12, EAST));
                      paths.addWaypoint(Pose2D(50, MAXY - 12, EAST));
                      paths.addWaypoint(Pose2D(12, MAXY - 12, EAST));
                      paths.addWaypoint(Pose2D(18, MAXY - 3, EAST));
                      paths.addWaypoint(Pose2D(10, MAXY - 3, EAST));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 9:
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(12, MAXY - 18, EAST));
                      paths.addWaypoint(Pose2D(50, MAXY - 18, EAST));
                      paths.addWaypoint(Pose2D(12, MAXY - 18, EAST));
                      paths.addWaypoint(Pose2D(18, MAXY - 3, EAST));
                      paths.addWaypoint(Pose2D(10, MAXY - 3, EAST));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 10:
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(12, MAXY - 24, EAST));
                      paths.addWaypoint(Pose2D(40, MAXY - 24, EAST));
                      paths.addWaypoint(Pose2D(12, MAXY - 24, EAST));
                      paths.addWaypoint(Pose2D(18, MAXY - 3, EAST));
                      paths.addWaypoint(Pose2D(10, MAXY - 3, EAST));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 11:
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(12, MAXY - 30, EAST));
                      paths.addWaypoint(Pose2D(50, MAXY - 30, EAST));
                      paths.addWaypoint(Pose2D(12, MAXY - 30, EAST));
                      paths.addWaypoint(Pose2D(18, MAXY - 3, EAST));
                      paths.addWaypoint(Pose2D(10, MAXY - 3, EAST));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index = 19;
                    }
                    break;
                  case 19:  // sweep north
                    if (!command_set) {
                      paths.addWaypoints(HardBox::setupCaveSweepNorth);
                      paths.addWaypoints(HardBox::caveSweepNorth);
                      paths.addWaypoints(HardBox::caveSweepReturn);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 1000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 20:  // sweep south
                    if (!command_set) {
                      paths.addWaypoints(HardBox::setupCaveSweepSouth);
                      paths.addWaypoints(HardBox::caveSweepSouth);
                      paths.addWaypoints(HardBox::caveSweepReturn);
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      drive.SetPosition(Pose2D(12, MAXY - 6, EAST));
                      transferMotor.Set(255);
                      static elapsedMillis timer = 0;
                      while (timer < 3000) {
                        transferMotor.Write();
                      }
                      delay(1000);
                      transferMotor.Set(0);
                      transferMotor.Write();
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 21:
                    if (!command_set) {
                      paths.addWaypoint(Pose2D(31.5, 6, NORTH));
                      command_set = true;
                      command_timer = 0;
                    }
                    if (paths.executePath()) {
                      command_set = false;
                      path_index++;
                    }
                    break;
                  case 22:
                    for (int i = 0; i < 7; i++) {
                      rgb.setSectionSolidColor(i, GOLD);
                    }
                    break;
                }
                /*static bool path_clear = false;
                path_clear = paths.executePath();
                if (path_clear)
                {
                  paths.addWaypoints(Paths::andrew_waypoint);
                }*/
              }
              // Drive update
              if (path_index != 22) {
                drive.Set(drive.Step());
              } else {
                reset();
              }
              static elapsedMillis timer = 0;
              if (timer > 5500) {
                timer = 0;
              } else if (timer > 5000) {
                intakeMotor.Set(-50);
              } else {
                intakeMotor.Set(150);
              }
              sorter.Update();
              transferMotor.Write();
              intakeMotor.Write();
              transferMotor.Write();
              drive.Write();
              break;
            }
          }
          break;
        }

        case REMOTE_CON: {
          GlobalRead();
          GlobalUpdate();
          GlobalStats();
          if (GlobalPrint()) {
            drive.PrintController(Serial, false);
          }
          drive.ReadAll(gyro.GetGyroData()[0]);  // Update encoders every frame

          if (update10Available) {
            update10Available = false;
            if (buttons.GetStates()[0] && RESET_AVAILABLE)  // Reset function
            {
              reset();
            }
            for (int i = 0; i < 5; i++)  // Write RGB
            {
              rgb.setSectionSolidColor(i, GOLD);
            }
          }

          if (update200Available) {
            update200Available = false;
            update200hz = 0;

            // --- Drive Update ---
            Pose2D speedPose =
                CalculateRCVector(true);  // drive.ConstrainNewSpeedPose(CalculateRCVector(true));
            drive.SetTargetByVelocity(speedPose);
            Pose2D hihi = drive.Step();
            drive.Set(hihi);

            // --- Other systems update ---
            sorter.Update();  // Update sorter
            int intakeSpeed = rc.Get(5);
            if (abs(intakeSpeed) < 30) {
              intakeSpeed = 0;
            }
            intakeMotor.Set(intakeSpeed);

            (rc.Get(6) == -255) ? mandibles.CloseLeft() : mandibles.OpenLeft();
            (rc.Get(7) == -255) ? mandibles.CloseRight() : mandibles.OpenRight();
            if (rc.Get(8) == -255) {
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
      break;
    }
    case STOPPED: {
      delay(10);
      break;
    }
    case ERROR: {
      for (int i = 0; i < 7; i++) {
        rgb.setSectionSolidColor(i, RED);
      }
      delay(10);
      break;
    }
  }
}

void GlobalRead() {
  static elapsedMillis read10hz = 0;
  if (read10hz > 100) {
    read10hz = 0;
    light.Update();
    halls.Update();
    buttons.Update();
  }

  static elapsedMillis read20hz = 0;
  if (read20hz > 50) {
    read20hz = 0;
    tofs.Update();
  }

  static elapsedMillis read100hz = 0;
  if (read100hz > 10) {
    read100hz = 0;
    gyro.Update();
  }

  static elapsedMillis read200hz = 0;
  if (read200hz > 5) {
    read200hz = 0;
    rc.Update();
  }
}

void GlobalUpdate() {
  static elapsedMillis rgbUpdate = 0;
  if (rgbUpdate > 20) {
    rgbUpdate = 0;
    rgb.Update();
    servos.Update();
  }
}

bool GlobalPrint() {
  static elapsedMillis printTimer = 0;
  if (printTimer > 200) {
    printTimer = 0;
    Serial.print("State: ");
    Serial.println(STATE);
    Serial.print("Progam: ");
    Serial.println(ROBOT_CONTROL_TYPE);
    Serial.print("Path Selection: ");
    Serial.println(PROGRAM_SELECTION);
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
  } else {
    return false;
  }
}

bool GlobalStats() {
  static elapsedMillis fpsTimer = 0;
  static long cycles = 0;
  cycles++;
  static long lastCycles = 0;
  if (fpsTimer >= 1000) {
    fpsTimer = 0;
    lastCycles = cycles;
    fps = lastCycles;
    cycles = 0;
    return true;
  } else {
    return false;
  }
}

Pose2D CalculateRCVector(bool positionControl) {
  float x = map((float)constrain(rc.Get(1), -255, 255), -255, 255, -1,
                1);  // RPot Y (x direction at 0 deg)
  float y = map((float)constrain(rc.Get(0), -255, 255), -255, 255, 1,
                -1);  // RPot X, rev (y direction at 0 deg)
  float theta = map((float)constrain(rc.Get(3), -255, 255), -255, 255, -1, 1);  // LPot X
  float yaw = gyro.GetGyroData()[0];
  return drive.CalculateRCVector(x, y, theta, yaw, positionControl);
}

void reset() {
  startup_middle_hook();
  WRITE_RESTART(0x05FA0004);
  // unused_interrupt_vector();
  servos.Detach();
}

FLASHMEM void startup_middle_hook(void) {
  for (uint8_t i = 0; i < CORE_NUM_DIGITAL; i++) {
    pinMode(i, INPUT_DISABLE);
  }
}

void updateDips() {
  buttons.Update();
  dips = buttons.GetStates();
  USING_EXTERNAL_CONTROL = dips[2];
  CONTROLLED_BY_PI = !dips[3];
}
