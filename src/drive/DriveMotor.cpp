#include "DriveMotor.h"
#include "Arduino.h"
#include "Print.h"

int DriveMotor::encoderNum = 1;

DriveMotor::DriveMotor(int kPWM, int kCW, int kENC_A, int kENC_B, bool rev)
  : kPWM(kPWM), kCW(kCW), kENC_A(kENC_A), kENC_B(kENC_B), kRev(rev), pwmout(0), cwout(true), enc(0) {
    if(encoderNum <= 4){
      encoder = new QuadEncoder(encoderNum, kENC_A, kENC_B);
    }
    else{
      encoder = NULL;
    }
    encoderNum++;
  }

void DriveMotor::Begin() {
  pinMode(kCW, OUTPUT);
  analogWriteFrequency(kPWM, 36621.09);
  if(encoder){
    encoder->setInitConfig();
    encoder->EncConfig.decoderWorkMode = 1;
    encoder->init();
  }
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
  if(encoder) {
      enc = encoder->read();
  }
}

int DriveMotor::GetEnc() {
  if(kENC_A == -1 || kENC_B == -1) {
    return 0;
  }
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
    output.print(F(", kENC_A: "));
    output.print(kENC_A);
    output.print(F(", kENC_B: "));
    output.print(kENC_B);
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


