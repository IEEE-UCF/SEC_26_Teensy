#ifndef ServoHandler_h
#define ServoHandler_h

#include <Servo.h>
#include <Arduino.h>

#define SERVO_COUNT 3
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
    void WriteServoAngle(int index, int angle);
    void Write();
    int *Get();

    friend Print &operator<<(Print &output, const ServoHandler &handler);

private:
    Servo *servos;
    int *anglesWrite;
    int *kServo;
    int numServos;
};

#endif