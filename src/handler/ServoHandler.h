#ifndef ServoHandler_h
#define ServoHandler_h

#include <Servo.h>
#include <Arduino.h>
#include <Print.h>

#define RANGE_LOW 1000
#define RANGE_HIGH 2000
#define ANGLE_LOW 0
#define ANGLE_HIGH 180
class ServoHandler
{
public:
    ServoHandler(int *kServo, int numServos);

    void Setup();
    void Set(int *anglesWrite);
    void Write();
    void WriteServoAngle(int index, int angle);
    int *Get();
    void PrintInfo(Print &output, bool printConfig = false) const;
    friend Print &operator<<(Print &output, const ServoHandler &handler);

private:
    Servo *servos;
    int *anglesWrite;
    int *kServo;
    int numServos;
};

#endif