/*
  ===== DEPENDENCIES =====
*/

//EX: #include "SelectorHandler.h"

/* --- Pinout ---
 0 - xxx
*/

/*
  ===== DEFINITIONS =====
*/

// --- Constants ---
//EX: #define PWM_LOW 980

// --- Input Pins ---
//EX: const int kSelector[2] = {11, 12};

// --- Output Pins ---
//EX: const int kRGB[3] = {8, 7, 4};               // RGB Pins

/*
  ===== SETUP =====
*/

// --- Input Handlers ---
//EX: RFHandler rf(kRF[0], kRF[1]);


// --- Output Handlers ---
//EX: DriveHandler drive(kMotors[0][0], kMotors[0][1], kMotors[1][0], kMotors[1][1], true, false);

void setup() {
  Serial.begin(115200);  //Begin serial

  // --- Begin Handlers ---
  //EX: drive.begin();
}

void loop() {
  // --- Input Handling ---
  //EX: rf.Receive();

  // --- Input Parsing ---
  //EX: rf.Parse(true);
  //EX: drive.Set(rf.ly, rf.rx, rf.b1, rf.b2);

  // -- Output ---
  //EX: drive.Update();
}
