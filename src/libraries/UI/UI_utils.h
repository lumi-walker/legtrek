#ifndef UI_UTILS_H
#define UI_UTILS_H

#include "ui_pin_assignments.h"
/*
	Contains parameters and functions related to the UI
*/


// SET SPEED PARAMS
float dSpeed = 0.1; //speed change


//JOYSTICK PARAMS
double angRead = 0; //angle of pitch
double rRead =  0; //magnitude of pitch
double rDeadBand = 0.1;
double forwardBearing = PI/2;
double angleBand = 20*PI/180; // 10 degrees to 180

// joystick angular bounds for moving forward
double forward_min = forwardBearing - angleBand;
double forward_max = forwardBearing + angleBand;

// joystick angular bounds for right turn
double rturn_min = 0 - angleBand;
double rturn_max = 0 + angleBand;

// joystick angular bounds for left turn
double lturn_min = PI - angleBand;
double lturn_max = PI + angleBand;

void buttonBlink(int whichLED) {
  if (millis() % 2000 < 1000) {
    digitalWrite(whichLED,HIGH);
  }
  else {
    digitalWrite(whichLED,LOW);
  }
}

void readJoystick() {
  int xRead = -(analogRead(xJS)-512);
  int yRead = (analogRead(yJS)-512);
  angRead = atan2(yRead,xRead);
  if (angRead < 0){angRead == angRead+2*PI;}
  double xR = (double)xRead;
  double yR = (double)yRead;
  xR = xR/512;
  yR = yR/512;
  rRead = sqrt(pow(xR,2) + pow(yR,2));
}


#endif