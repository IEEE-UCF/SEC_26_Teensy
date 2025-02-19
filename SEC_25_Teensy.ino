/*
  ===== DEPENDENCIES =====
*/
// EX: #include "SelectorHandler.h"
#include <Arduino.h>
#include "Pose2D.h"
#include "VectorRobotDrive.h"
#include "ButtonHandler.h"
#include "ServoHandler.h"

/* --- Pinout ---
 0 - xxx
*/

/*
  ===== DEFINITIONS =====
*/

// --- Constants ---
// EX: #define PWM_LOW 980
#define DRIVEMOTOR_COUNT 3
#define NONDRIVEMOTOR_COUNT 2
#define SERVO_COUNT 3 
#define BUTTON_COUNT 2

// --- Motor Setup ---
const int kPWM[DRIVEMOTOR_COUNT] = {3, 5, 6}; //TODO : set
const int kCW[DRIVEMOTOR_COUNT] = {2, 4, 7}; //TODO : set
const int kENC[DRIVEMOTOR_COUNT] = {A0, A1, A2}; //TODO : set
const bool rev[DRIVEMOTOR_COUNT] = {false, false, false}; //TODO : set

const int nkPWM[NONDRIVEMOTOR_COUNT] = {1, 2}; //TODO : set
const int nkCW[NONDRIVEMOTOR_COUNT] = {1, 2}; //TODO : set
const int nrev[NONDRIVEMOTOR_COUNT] = {1, 2}; //TODO : set


// --- Other Input Pins ---
// EX: const int kSelector[2] = {11, 12};
const int kButton[BUTTON_COUNT] = {0, 1}; //TODO : set

// --- Other Output Pins ---
// EX: const int kRGB[3] = {8, 7, 4};               // RGB Pins
const int kServo[SERVO_COUNT] = {10, 11, 12}; //TODO : set

/*
  ===== SETUP =====
*/

// --- Input Handlers ---
// EX: RFHandler rf(kRF[0], kRF[1]);
ButtonHandler buttons(kButton, BUTTON_COUNT);

// --- Output Handlers ---
// EX: DriveHandler drive(kMotors[0][0], kMotors[0][1], kMotors[1][0], kMotors[1][1], true, false);
VectorRobotDrive robotDrive(kPWM, kCW, kENC, rev, DRIVEMOTOR_COUNT);
ServoHandler servos(kServo, SERVO_COUNT);
DriveMotor intakeMotor(nkPWM[0], nkCW[0], -1, nrev[0]);
DriveMotor sorterMotor(nkPWM[1], nkCW[1], -1, nrev[1]);


// --- Two-Way Handlers ---
//Serial handler





void setup()
{
  Serial.begin(115200); // Begin serial
  Serial.println();
  // --- Begin Handlers ---
  // EX: drive.begin();
  buttons.Setup();
  servos.Setup();

  robotDrive.PrintInfo(Serial, true);
  servos.PrintInfo(Serial, true);
  intakeMotor.PrintInfo(Serial, true);
  sorterMotor.PrintInfo(Serial, true);
}

void loop()
{
  // --- Input Handling ---
  // EX: rf.Receive();
  buttons.Read();

  // --- Input Parsing ---
  // EX: rf.Parse(true);

  // --- Set Outputs ---
  int speeds[3] = {0, 0, 0};
  robotDrive.Set(speeds);
  intakeMotor.Set(0);
  sorterMotor.Set(0);

  // -- Write Outputs ---
  // EX: drive.Update();
  robotDrive.Write();
  intakeMotor.Write();
  sorterMotor.Write();

  //alternate method for printing info by overloading << operator
  Serial << robotDrive << intakeMotor << sorterMotor << servos;

  delay(250);
}
