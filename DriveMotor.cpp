#include "DriveMotor.h"
#include "Arduino.h"

DriveMotor::DriveMotor(int kPWM, int kCW, int kENC, bool reversed = false)
{
  _kPWM = kPWM;
  _kCW = kCW;
  _kENC = kENC;
  _rev = reversed;
}

void DriveMotor::Set(int speed) {
  pwmout = map(constrain(speed, -IN_MAX, IN_MAX), -IN_MAX, IN_MAX, -PWM_MAX, PWM_MAX);
}

void DriveMotor::Update() {
  analogWrite(_kPWM, pwmout);
}

void DriveMotor::Read() {

}