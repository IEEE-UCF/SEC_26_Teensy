#include "Arduino.h"
#include <SoftwareSerial.h>

#ifndef RFHandler_h
#define RFHandler_h

class RFHandler
{
public:
    RFHandler(uint8_t receivePin, uint8_t transmitPin);
    void Begin();
    void Receive();
    void Parse(bool verbose = false);
    void Show();
    void Write(int rgb);

  int lx = 0;
  int ly = 0;
  int lsw = 0;
  int rx = 0;
  int ry = 0;
  int rsw = 0;
  int b1 = 0;
  int b2 = 0;

private:
    const byte numChars = 64;
    char receivedChars[64];
    char tempChars[64];

    boolean newData = false;
    SoftwareSerial HC12;
  int blx[5] = {0};
  int bly[5] = {0};
  int blsw[5] = {0};
  int brx[5] = {0};
  int bry[5] = {0};
  int brsw[5] = {0};
  int bb1[5] = {0};
  int bb2[5] = {0};

  int getavg(int arr[5]);
  void pop(int arr[5]);
  void verify(int arr[5], int* value);
};

#endif