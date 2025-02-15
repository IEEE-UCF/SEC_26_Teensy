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
// #define MOTOR_COUNT 4  VectorRobotDrive.
// #define SERVO_COUNT 3  ServoHandler.h
#define BUTTON_COUNT 2

// --- Motor Setup ---
const int kPWM[MOTOR_COUNT] = {3, 5, 6, 9}; //TODO
const int kCW[MOTOR_COUNT] = {2, 4, 7, 8}; //TODO
const int kENC[MOTOR_COUNT] = {A0, A1, A2, A3}; //TODO
const bool rev[MOTOR_COUNT] = {false, false, false, false}; //TODO

// --- Other Input Pins ---
// EX: const int kSelector[2] = {11, 12};
const int kButton[BUTTON_COUNT] = {0, 1}; //TODO

// --- Other Output Pins ---
// EX: const int kRGB[3] = {8, 7, 4};               // RGB Pins
const int kServo[SERVO_COUNT] = {10, 11, 12};

/*
  ===== SETUP =====
*/

// --- Input Handlers ---
// EX: RFHandler rf(kRF[0], kRF[1]);
ButtonHandler buttons(kButton, BUTTON_COUNT);
//BNO055 handler
//Hall Effect handler
//VL53L0X handler

// --- Output Handlers ---
// EX: DriveHandler drive(kMotors[0][0], kMotors[0][1], kMotors[1][0], kMotors[1][1], true, false);
VectorRobotDrive robotDrive(kPWM, kCW, kENC, rev);
ServoHandler servos(kServo, SERVO_COUNT);

// --- Two-Way Handlers ---
//Serial handler





void setup()
{
  Serial.begin(115200); // Begin serial
  Serial.println();
  // --- Begin Handlers ---
  // EX: drive.begin();
  // (symbolic) robotDrive.Setup();
  buttons.Setup();
  Serial << robotDrive;
  Serial << servos;
}

void loop()
{
  // --- Input Handling ---
  // EX: rf.Receive();
  buttons.Read();

  // --- Input Parsing ---
  // EX: rf.Parse(true);
  // EX: drive.Set(rf.ly, rf.rx, rf.b1, rf.b2);
  robotDrive.Set(Pose2D(10,10,10));
  Serial << robotDrive.GetPose();

  // -- Output ---
  // EX: drive.Update();
  delay(1000);
}
