#include "DriveMotor.h"
#include "Arduino.h"

DriveMotor::DriveMotor(int kPWM, int kCW, int kENC, bool rev) : kPWM(kPWM), kCW(kCW), kENC(kENC), kRev(kRev) {}

// TODO: obviously, the pwmout needs to be verified and checked
void DriveMotor::Set(int speed)
{
  if (kRev)
  {
    speed = -speed;
  }
  pwmout = map(constrain(speed, -SPEED_MAX, SPEED_MAX), -SPEED_MAX, SPEED_MAX, -PWM_MAX, PWM_MAX);
  if (speed < 0)
  {
    cwout = false;
  }
  else
  {
    cwout = true;
  }
}

void DriveMotor::Read(int *reading)
{
  // TODO: implement encoder
  *reading = 0;
}

void DriveMotor::Write()
{
  analogWrite(kPWM, pwmout);
  digitalWrite(kCW, cwout);
}