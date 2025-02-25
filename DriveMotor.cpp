#include "DriveMotor.h"
#include "Arduino.h"
#include "Print.h"

DriveMotor::DriveMotor(int kPWM, int kCW, int kENC, bool rev)
  : kPWM(kPWM), kCW(kCW), kENC(kENC), kRev(rev), pwmout(0), cwout(true), enc(0) {}

void DriveMotor::Begin() {
  pinMode(kCW, OUTPUT);
  analogWriteFrequency(kPWM, 36621.09);
}

void DriveMotor::Set(int speed) {
  if (kRev) {
    speed = -speed;
  }
  pwmout = map(abs(speed), 0, SPEED_MAX, PWM_MAX, 0);
  cwout = (speed >= 0);
}

void DriveMotor::ReadEnc() {
  // TODO: implement encoder
  if (kENC == -1) {
    enc = 0;
    return;
  }
  enc = 0;
}

int DriveMotor::GetEnc() {
  return enc;
}

void DriveMotor::Write() {
  analogWrite(kPWM, pwmout);
  digitalWrite(kCW, cwout);
}
void DriveMotor::PrintInfo(Print &output, bool printConfig) const {
  if (printConfig) {
    output.print(F("DriveMotor Configuration: "));
    output.print(F("kPWM: "));
    output.print(kPWM);
    output.print(F(", kCW: "));
    output.print(kCW);
    output.print(F(", kENC: "));
    output.print(kENC);
    output.print(F(", kRev: "));
    output.println(kRev ? F("True") : F("False"));
  } else {
    output.print(F("DriveMotor PWM Output: "));
    output.print(pwmout);
    output.print(F(", CW Output: "));
    output.print(cwout ? F("True") : F("False"));
    output.print(F(", Encoder: "));
    output.println(enc);
  }
}

Print &operator<<(Print &output, const DriveMotor &motor) {
  motor.PrintInfo(output, false);
  return output;
}


