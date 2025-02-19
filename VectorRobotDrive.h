#ifndef VectorRobotDrive_h
#define VectorRobotDrive_h

#include "Pose2D.h"
#include "DriveMotor.h"
#include "Arduino.h"
#include <Print.h>


class VectorRobotDrive
/*
    ASSUME MOTORS ARE IN THE CONFIGURATION:
    left upper, left bottom, right bottom, right upper
*/
{
public:
  VectorRobotDrive(int kPWM[], int kCW[], int kENC[], bool rev[], int numMotors);
  void Set(const Pose2D &speedPose);
  void Set(int motorDirectSpeed[]);
  void SetIndex(int motorDirectSpeed, int index);
  void ReadEnc();
  Pose2D GetPose();
  int *GetEnc();
  void Write();
   void PrintInfo(Print &output, bool printConfig = false) const;
    friend Print &operator<<(Print &output, const VectorRobotDrive &drive);

private:
  Pose2D speedPose;
  int numMotors;
  DriveMotor **motors; // Changed to pointer to pointer
  int* enc;
  int* kPWM;
  int* kCW;
  int* kENC;
  bool* rev;
};

#endif