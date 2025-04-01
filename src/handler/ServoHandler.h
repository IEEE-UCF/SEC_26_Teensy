/*
Aldem Pido - 4/1/25
ServoHandler.h - Controls servos on a robot
*/
#ifndef ServoHandler_h
#define ServoHandler_h

#include <Servo.h>
#include <Arduino.h>
#include <Print.h>

#define RANGE_LOW 1000
#define RANGE_HIGH 2000
#define ANGLE_LOW 0
#define ANGLE_HIGH 180
#define DEFAULT_MOVEMENT_SPEED 30 // Degrees per second

class ServoHandler
{
public:
    ServoHandler(int *kServo, int numServos);

    void Begin();
    void Set(int *anglesWrite);
    void Write();
    void WriteServoAngle(int index, int angle);
    void WriteServoAngleSmooth(int index, int angle, int speed = DEFAULT_MOVEMENT_SPEED);
    int *Get();
    void Attach();
    void Detach();
    void PrintInfo(Print &output, bool printConfig = false) const;
    void Update();            // Must be called in loop to handle smooth movements
    void SetSpeed(int speed); // Set global movement speed in degrees per second
    friend Print &operator<<(Print &output, const ServoHandler &handler);

private:
    Servo *servos;
    int *anglesWrite;            // Current commanded angles
    int *currentAngles;          // Current actual angles (during transitions)
    int *targetAngles;           // Target angles for smooth movement
    unsigned long *lastMoveTime; // Last time each servo was moved
    int *kServo;                 // Servo pins
    int numServos;
    bool *attached;
    int movementSpeed; // Degrees per second
};

#endif