/*
  ===== DEPENDENCIES =====
*/

//#include <RH_ASK.h>
//#ifdef RH_HAVE_HARDWARE_SPI
//#include <SPI.h> // Not actually used but needed to compile
//#endif
//#include <Wire.h>
#include <RGBHandler.h>
#include "LineHandler.h"
#include "SelectorHandler.h"
//#include "TOFHandler.h"
//#include "MPUHandler.h"
#include "DriveHandler.h"
#include <SoftwareSerial.h>
#include "RFHandler.h"

/* --- Pinout ---
  0:  TX/RX
  1:  TX/RX
  2:  RF Data
  3:  Motor Out 1 PWM
  4:  RGB Green
  5:  Motor Out 2 PWM
  6:  Motor Out 3 PWM
  7:  RGB Blue
  8:  RGB Red
  9:  Motor Out 4 PWM
  10: Selector 2^0
  11: Selector 2^1
  12: Selector 2^2
  13: Selector 2^3
  A0: Line 1
  A1: Line 2
  A2: Line 3
  A3: Line 4
  A4: SDA
  A5: SCL
  A6: x
  A7: x
  --- I2C Breakout Channels ---
  0:  Gyro
  1:  TOF 1
  2:  TOF 2
  3:  TOF 3
  4:  TOF 4
  5:  TOF 5
  6:  TOF 6
  7:  x
*/

/*
  ===== DEFINITIONS =====
*/

// --- Constants ---
#define PWM_LOW 980  //Radiolink RF Receiver
#define PWM_HIGH 2000
//#define DIST_THRESH 200
//#define STOP_THRESH 200

// --- Input Pins ---
const int kSelector[2] = { 11, 12 };      //Selector Pins
const int kLine[4] = { A0, A1, A2, A3 };  //Line Pins
const int kRF[2] = { 2, 3 };              //RF Pins HC-12 TX Pin, HC-12 RX Pin
//const int cTOF[5] = { 1, 0, 5, 6, 7 };        //TOF Channels
//const int cMPU = 2;                           //MPU Channel

// --- Output Pins ---
const int kRGB[3] = { 8, 7, 4 };                    //RGB Pins
const int kMotors[2][2] = { { 6, 9 }, { 5, 10 } };  //Motor Pins

// --- Program, RF Variables ---

/*
  ===== SETUP =====
*/

// --- Input Handlers ---
SelectorHandler sel(kSelector[0], kSelector[1]);
LineHandler line(kLine[0], kLine[1], kLine[2], kLine[3]);
RFHandler rf(kRF[0], kRF[1]);
//TOFHandler tof(cTOF[0], cTOF[1], cTOF[2], cTOF[3], cTOF[4]);
//MPUHandler mpu(cMPU);

// --- Output Handlers ---
RGBHandler rgb(kRGB[0], kRGB[1], kRGB[2]);
DriveHandler drive(kMotors[0][0], kMotors[0][1], kMotors[1][0], kMotors[1][1], true, false);

void setup() {
  //Normal startup. Make sure wire only begins once
  Serial.begin(115200);

  //Short routine to indicate selection. Bring selection pins to ground to select
  int x = sel.ReadSel();
  Serial.println(x);
  if (x == 0) {
    rgb.setRGB(7);
  } else {
    rgb.setRGB(x);
  }
  delay(3000);
  rgb.clearRGB();

  //line.Begin() Symbolic. LineHandler does not have a begin function
  rf.Begin();
  //drive.Begin() Symbolic. DriveHandler does not have a begin function
}

void loop() {
  /*if (sel.ReadSel() == 1) {
    // --- update inputs ---
    line.Update();

    // --- input definitions ---
    int rLine[4];

    // --- get inputs ---
    Serial.print(F("Lines "));
    for (int i = 0; i < 4; i++) {
      rLine[i] = line.Get(i);
      Serial.print(rLine[i]);
      Serial.print(F(" "));
    }
    Serial.print(F(" | "));

    Serial.println();


    // --- statements ---

    // --- outputs ---

    rgb.setRGB(1);
    drive.Set(150, 0);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(2);
    drive.Set(-150, 0);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(3);
    drive.Set(0, 150);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(4);
    drive.Set(0, -150);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(5);
    drive.Set(150, 150);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(6);
    drive.Set(-150, -150);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(7);
    drive.Set(100, 150);
    drive.Update();
    delay(1000);

    stop();
    // --- update outputs ---
    //drive.Update();
  }
  if (sel.ReadSel() == 2) {
    // --- update inputs ---
    line.Update();

    // --- input definitions ---
    int rLine[4];

    // --- get inputs ---
    Serial.print(F("Lines "));
    for (int i = 0; i < 4; i++) {
      rLine[i] = line.Get(i);
      Serial.print(rLine[i]);
      Serial.print(F(" "));
    }
    Serial.print(F(" | "));

    Serial.println();


    // --- statements ---

    // --- outputs ---

    rgb.setRGB(1);
    drive.Set(150, 150);
    drive.Update();
    delay(1000);

    stop();

    rgb.setRGB(2);
    drive.Set(-150, -150);
    drive.Update();
    delay(1000);

    stop();

    // --- update outputs ---
    //drive.Update();
  }
  if (sel.ReadSel() == 4) {
    // --- update inputs ---
    line.Update();

    // --- input definitions ---
    int rLine[4];

    // --- get inputs ---
    Serial.print(F("Lines "));
    for (int i = 0; i < 4; i++) {
      rLine[i] = line.Get(i);
      Serial.print(rLine[i]);
      Serial.print(F(" "));
    }
    Serial.print(F(" | "));

    Serial.println();


    // --- statements ---

    // --- outputs ---

    rgb.setRGB(1);
    drive.Set(255, 255);
    drive.Update();
    delay(750);

    stop();

    rgb.setRGB(2);
    drive.Set(-100, -100);
    drive.Update();
    delay(2000);

    stop();

    // --- update outputs ---
    //drive.Update();
  }*/
  if (sel.ReadSel() == 0) {
    rf.Receive();
    rf.Parse(true);
    drive.Set(rf.ly, rf.rx, rf.b1, rf.b2);
    static int lastRGB = 0;
    static bool RGBLockout = false;
    int writeRGB;
    if (rf.lsw == 0 && rf.rsw == 0) {
      RGBLockout = !RGBLockout;
      delay(300);
    }
    if (rf.b2 == 0) {
      writeRGB = 6;
    } else if (rf.b1 == 0) {
      writeRGB = 2;
    } else if (rf.ly > 10) {
      writeRGB = 5;
    } else if (rf.ly < -10) {
      writeRGB = 1;
    } else if (abs(rf.rx) > 10) {
      writeRGB = 3;
    } else {
      writeRGB = 4;
    }

    if (RGBLockout) {
      writeRGB = 0;
    }

    if (lastRGB != writeRGB) {
      rf.Write(writeRGB);
      lastRGB = writeRGB;
      rgb.setRGB(writeRGB);
    }
    
    drive.Update();
  }
}

/*void stop() {
  rgb.setRGB(5);
    drive.Set(0, 0);
    drive.Update();
    delay(1000);
}*/
