/*
  ===== DEPENDENCIES =====
*/
// EX: #include "SelectorHandler.h"
#include <Arduino.h>
#include "Pose2D.h"
#include "PoseRobotDrive.h"

/* --- Pinout ---
 0 - xxx
*/

/*
  ===== DEFINITIONS =====
*/

// --- Constants ---
// Specific constants should be held in their respective files.
// Only use global constants here
// EX: #define PWM_LOW 980

// --- Motor Setup ---
// TODO: update motor pins
// note: motor_count is from VectorRobotDrive
int kPWM[MOTOR_COUNT] = {3, 5, 6, 9};
int kCW[MOTOR_COUNT] = {2, 4, 7, 8};
int kENC[MOTOR_COUNT] = {A0, A1, A2, A3};
bool rev[MOTOR_COUNT] = {false, false, false, false};

// --- Other Input Pins ---
// EX: const int kSelector[2] = {11, 12};

// --- Other Output Pins ---
// EX: const int kRGB[3] = {8, 7, 4};               // RGB Pins

/*
  ===== SETUP =====
*/

// --- Input Handlers ---
// EX: RFHandler rf(kRF[0], kRF[1]);

// --- Output Handlers ---
// EX: DriveHandler drive(kMotors[0][0], kMotors[0][1], kMotors[1][0], kMotors[1][1], true, false);
VectorRobotDrive robotDrive(kPWM, kCW, kENC, rev);

void setup()
{
  Serial.begin(115200); // Begin serial

  // --- Begin Handlers ---
  // EX: drive.begin();
}

void loop()
{
  // --- Input Handling ---
  // EX: rf.Receive();

  // --- Input Parsing ---
  // EX: rf.Parse(true);
  // EX: drive.Set(rf.ly, rf.rx, rf.b1, rf.b2);

  // -- Output ---
  // EX: drive.Update();
}
