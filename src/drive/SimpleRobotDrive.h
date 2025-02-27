#ifndef SimpleRobotDrive_h
#define SimpleRobotDrive_h

#include "DriveMotor.h"
#include "Arduino.h"
#include <Print.h>
#include "MOTORCONFIG.h"

class SimpleRobotDrive
/*
    ASSUME MOTORS ARE IN THE CONFIGURATION:
    left upper, left bottom, right bottom, right upper
*/
{
public:
  SimpleRobotDrive(int kPWM[], int kCW[], int kENC_A[], int kENC_B[], bool rev[], int numMotors);
  void Set(int motorDirectSpeed[]);
  void Begin();
  void SetIndex(int motorDirectSpeed, int index);
  void ReadEnc();
  int *GetEnc();
  void Write();
  void PrintInfo(Print &output, bool printConfig = false) const;
  friend Print &operator<<(Print &output, const SimpleRobotDrive &drive);

protected:
  int numMotors;
  DriveMotor **motors; // Changed to pointer to pointer
  int *enc;
  int *kPWM;
  int *kCW;
  int *kENC_A;
  int *kENC_B;
  bool *rev;
};

#endif