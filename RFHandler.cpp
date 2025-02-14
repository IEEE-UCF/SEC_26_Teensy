#include "Arduino.h"
#include <SoftwareSerial.h>
#include "RFHandler.h"

RFHandler::RFHandler(uint8_t receivePin, uint8_t transmitPin)
  : HC12(receivePin, transmitPin) {
}

void RFHandler::Begin() {
  HC12.begin(38400);
}

void RFHandler::Receive() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '<';
  char endMarker = '>';
  char rc;

  while (HC12.available() > 0 && newData == false) {
    rc = HC12.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      } else {
        receivedChars[ndx] = '\0';  // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


void RFHandler::Parse(bool verbose = false) {
  if (newData == true) {
    strcpy(tempChars, receivedChars);  //protects receivedChars
    char* strtokIndx;
    //Serial.print(receivedChars);
    /*   int ch_lx = 0;
  int ch_ly = 0;
  int ch_lsw = 0;
  int ch_rx = 0;
  int ch_ry = 0;
  int ch_rsw = 0;
  int ch_b1 = 0;
  int ch_b2 = 0;*/
    //form <lx, ly, lsw, rx, ry, rsw, b1, b2>
    //pop(blx);
    strtokIndx = strtok(tempChars, ",");
    blx[0] = atoi(strtokIndx);

    //pop(bly);
    strtokIndx = strtok(NULL, ",");
    bly[0] = atoi(strtokIndx);

    //pop(blsw);
    strtokIndx = strtok(NULL, ",");
    blsw[0] = atoi(strtokIndx);

    //pop(brx);
    strtokIndx = strtok(NULL, ",");
    brx[0] = atoi(strtokIndx);

    //pop(bry);
    strtokIndx = strtok(NULL, ",");
    bry[0] = atoi(strtokIndx);

    //pop(brsw);
    strtokIndx = strtok(NULL, ",");
    brsw[0] = atoi(strtokIndx);

    //pop(bb1);
    strtokIndx = strtok(NULL, ",");
    bb1[0] = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    bb2[0] = atoi(strtokIndx);

    strtokIndx = strtok(NULL, ",");
    blx[1] = atoi(strtokIndx);

    //pop(bly);
    strtokIndx = strtok(NULL, ",");
    bly[1] = atoi(strtokIndx);

    //pop(blsw);
    strtokIndx = strtok(NULL, ",");
    blsw[1] = atoi(strtokIndx);

    //pop(brx);
    strtokIndx = strtok(NULL, ",");
    brx[1] = atoi(strtokIndx);

    //pop(bry);
    strtokIndx = strtok(NULL, ",");
    bry[1] = atoi(strtokIndx);

    //pop(brsw);
    strtokIndx = strtok(NULL, ",");
    brsw[1] = atoi(strtokIndx);

    //pop(bb1);
    strtokIndx = strtok(NULL, ",");
    bb1[1] = atoi(strtokIndx);

    //pop(bb2);
    strtokIndx = strtok(NULL, ",");
    bb2[1] = atoi(strtokIndx);

    /*Serial.print("buffer");
    Serial.print(blx[0]);
    Serial.print(" ");
    Serial.print(bly[0]);
    Serial.print(" ");
    Serial.print(blsw[0]);
    Serial.print(" ");
    Serial.print(brx[0]);
    Serial.print(" ");
    Serial.print(bry[0]);
    Serial.print(" ");
    Serial.print(brsw[0]);
    Serial.print(" ");
    Serial.print(bb1[0]);
    Serial.print(" ");
    Serial.print(bb2[0]);
    Serial.print(" ");
    Serial.print(blx[1]);
    Serial.print(" ");
    Serial.print(bly[1]);
    Serial.print(" ");
    Serial.print(blsw[1]);
    Serial.print(" ");
    Serial.print(brx[1]);
    Serial.print(" ");
    Serial.print(bry[1]);
    Serial.print(" ");
    Serial.print(brsw[1]);
    Serial.print(" ");
    Serial.print(bb1[1]);
    Serial.print(" ");
    Serial.print(bb2[1]);
    Serial.print(" ");*/
    verify(blx, &lx);
    verify(bly, &ly);
    verify(blsw, &lsw);
    verify(brx, &rx);
    verify(bry, &ry);
    verify(brsw, &rsw);
    verify(bb1, &b1);
    verify(bb2, &b2);
    /*lx = getavg(blx);
    ly = getavg(bly);
    lsw = getavg(blsw);
    rx = getavg(brx);
    ry = getavg(bry);
    rsw = getavg(brsw);
    b1 = getavg(bb1);
    b2 = getavg(bb2);*/
    newData = false;
    if (verbose) {
      Show();
    }
  }
}

int RFHandler::getavg(int arr[5]) {
  int sum = 0;
  for(int i = 0; i < 5; i++) {
    sum += arr[i];
  }
  return sum/5;
}

void RFHandler::pop(int arr[5]) {
  for(int i = 0; i < 5 - 1; i++) {
    arr[i] = arr[i+1];
  }
}

void RFHandler::verify(int arr[5], int* value) {
  if(arr[0] == arr[1]) {
    *value = arr[0];
    //Serial.print("hi");
  }
}

void RFHandler::Write(int rgb) {
  String data = "<" + String(rgb) +"," + String(rgb) + ">";
  Serial.print("begin");
  HC12.print(data);
  Serial.print("write");
}


void RFHandler::Show() {
  Serial.print("Data: ");
  Serial.print(lx);
  Serial.print(" ");
  Serial.print(ly);
  Serial.print(" ");
  Serial.print(lsw);
  Serial.print(" ");
  Serial.print(rx);
  Serial.print(" ");
  Serial.print(ry);
  Serial.print(" ");
  Serial.print(rsw);
  Serial.print(" ");
  Serial.print(b1);
  Serial.print(" ");
  Serial.print(b2);
  Serial.println();
}