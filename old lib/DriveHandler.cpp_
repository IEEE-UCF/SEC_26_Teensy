#include "DriveHandler.h";
#include "Arduino.h";

DriveHandler::DriveHandler(int k1, int k2, int k3, int k4, bool rightrev = true, bool dualrev = true)
{
    kPWM[0] = k1;
    kPWM[1] = k2;
    kPWM[2] = k3;
    kPWM[3] = k4;
    write[0] = 0;
    write[1] = 0;
    write[2] = 0;
    write[3] = 0;
    _rightrev = rightrev;
    _dualrev = dualrev;
}

void DriveHandler::Set(int leftin, int rightin, int b1, int b2)
{
    leftin = constrain(leftin, -MAX_IN, MAX_IN);
    rightin = constrain(rightin, -MAX_IN, MAX_IN);
    int controlleft;
    int controlright;
    if(b2 == 0) {
      controlleft = leftin;
      controlright = leftin;
    } else if(b1 == 0) {
      controlleft = leftin*0.4;
      controlright = rightin*0.4;
    } else {
      controlleft = leftin*0.75;
      controlright = rightin*0.75;
    }
    if(controlleft < 0) {
      controlright = (controlleft * 0.85) + ( rightin * -1 * 0.2);
      controlleft = (controlleft * 0.85) + ( rightin  * 0.2);
    } else {
      if(b2 == 0) {
        controlright = controlleft + ( rightin * -1 * 0.85);
        controlleft = controlleft + ( rightin * 0.85);
      } else if(b1 == 0) {
        controlright = controlleft + ( rightin * -1 * 0.3);
        controlleft = controlleft + ( rightin  * 0.3);
      } else {
        controlright = controlleft + ( rightin * -1 * 0.5);
        controlleft = controlleft + ( rightin  * 0.5);
      }
    }
    controlleft = constrain(controlleft, -255, 255);
    controlright = constrain(controlright, -255, 255);


    //Serial.println(controlleft);
    //Serial.println(controlright);
    int scaledL = map(controlleft, -MAX_IN, MAX_IN, -255, 255);
    int scaledR = map(controlright, -MAX_IN, MAX_IN, -255, 255);
    if(abs(scaledL) < 5) {
      scaledL = 0;
    }
    if(abs(scaledR) < 5) {
      scaledR = 0;
    }
    if (_rightrev)
    {
        scaledR = -scaledR;
    }
    if(_dualrev) {
      scaledL = -scaledL;
      scaledR = -scaledR;
    }
    if (scaledL < 0)
    {
        write[0] = 0;
        write[1] = abs(scaledL);
    }
    else if (scaledL == 0)
    {
        write[0] = 0;
        write[1] = 0;
    }
    else
    {
        write[0] = abs(scaledL);
        write[1] = 0;
    }

    if (scaledR < 0)
    {
        write[2] = 0;
        write[3] = abs(scaledR);
    }
    else if (scaledR == 0)
    {
        write[2] = 0;
        write[3] = 0;
    }
    else
    {
        write[2] = abs(scaledR);
        write[3] = 0;
    }
}

void DriveHandler::Update()
{
    for (int i = 0; i < 4; i++)
    {
        analogWrite(kPWM[i], write[i]);
    }
}