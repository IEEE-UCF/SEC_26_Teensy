#include "DriveMotor.h"
#include "Arduino.h"

DriveMotor::DriveMotor(int kPWM, int kCW, int kENC, bool rev) : kPWM(kPWM), kCW(kCW), kENC(kENC), rev(rev) {}

void DriveMotor::Set(int _speed, bool _fwd)
{
  if (rev)
  {
    _fwd = !_fwd;
  }
  pwmout = map(constrain(_speed, -SPEED_MAX, SPEED_MAX), -SPEED_MAX, SPEED_MAX, -PWM_MAX, PWM_MAX);
  cwout = _fwd;
}

void DriveMotor::Read(float *reading)
{
  // TODO: implement encoder
  *reading = 0;
}

void DriveMotor::Write()
{
  analogWrite(kPWM, pwmout);
  digitalWrite(kCW, cwout);
}