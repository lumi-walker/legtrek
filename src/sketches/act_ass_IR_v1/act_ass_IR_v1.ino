
//use IR
//no integration with UI
//takes value from serial monitor
#include "Motor.h"

int CrossPin1 = 2;  // input pin for break beam sensor
int CrossPin2 = 3;
int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE

String readString;

//random def
int accset = 0;
int initflag = 0;
bool resetSpeed = false;
float velset = 1.5;
long steptime = 1000000; //1000000micro sec = 1s .
long starttime = 0;
long threshold = 200000;

bool decellTime = false;
long waitTime;
long startTime_decell = -1;


void setup() {
  motor_init();//pinmode
  analogWriteResolution(12);

  pinMode(CrossPin1, INPUT);
  attachInterrupt(digitalPinToInterrupt(CrossPin1), initstep, FALLING);
  pinMode(CrossPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(CrossPin2), initstep, FALLING);
  Serial.begin(115200);
  while (! Serial);
  //  delayMicroseconds(steptime); //aviod counting the initial time
  //  Serial.println("ready");

  delayMicroseconds(steptime);// to reset motor
  motor_ready();//on,holding,faststop
  analogWrite(DAC1, 4095 - accset);
  Serial.println("motor ready");
}

void loop() {

  checkinput();

  if (initflag) { //if interrupt happens
    faststopoff_all();
    setspeed(float(velset), bool(1));
    initflag = 0;
    Serial.println("start moving");
    resetSpeed = true;
  }

//  if (micros() - starttime > steptime && resetSpeed == true) {
//    waitTime = steptime;
//    decellTime = true;
//    startTime_decell = micros();
//    setspeed(float(0), bool(1));
//    resetSpeed = false;
//    Serial.println("Setting speed to zero");
//  }
//
//  else if (resetSpeed == false && micros() - starttime > steptime && decellTime == false) {
//    Serial.println("waiting");
//  }
//  else if (micros() - starttime < steptime) { //if no interrupt
//    Serial.println("moving");
//  }
//
//  else if (micros() - startTime_decell >= waitTime && decellTime == true) {
//    faststopon_all();
//    decellTime = false;
//    Serial.println("decelled");
//  }

if (micros() - starttime > steptime && resetSpeed == true){
  decel_to_zero(accset);
  resetSpeed = false;
}

}

void initstep() {
  //initiate timer for a step
  if (micros() - starttime > threshold) {
//    if (micros() - starttime > steptime) {
      initflag = 1;
//    }
    starttime = micros();//time when a step initiated
  }

}


void checkinput() {
  // take value from serial monitor and determine states accordingly
  // !!!all input from serial monitor must start with a number and end with a comma!!!
  if (Serial.available() > 0) { //if there's a input from serial monitor
    char c = Serial.read();  //gets one byte from serial buffer

    if (c == ',') {
      if (readString.length() > 1) {
        Serial.println(readString); //prints string to serial port out

        int n = readString.toInt();  //convert readString into a number

        if (readString.indexOf("a") > 0) {

          accset = n;
          analogWrite(DAC1, 4095 - accset);
          Serial.println("acc = " + String(accset));
        }
        else if (readString.indexOf("v") > 0) {
          velset = float(n) / 10.0f;
          Serial.println("vel = " + String(velset));
        }
        else if (readString.indexOf("s") > 0) {
          steptime = n * 1000;
          Serial.println("steptime = " + String(float(steptime / 1000000)));
        }
        else if (readString.indexOf("fs") > 0) {
          faststopon_all();
        }
        else {
          Serial.println("ERROR! Follow input Format!!! Previous input cleared!");
          readString = ""; //clears variable for new input
        }
        readString = ""; //clears variable for new input
      }
    }
    else {
      readString += c; //makes the string readString
    }

  }

}
