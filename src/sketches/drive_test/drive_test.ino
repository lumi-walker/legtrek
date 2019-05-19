#include "UI_utils.h"
#include "ui_pin_assignments.h"
#include "Motor.h"
//send input to M2 and reading outputs from M2
float speed_sp = 1;
double ang_sp = PI/2;
String readString;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
motor_init();
motor_ready();
}

void loop() {
  // put your main code here, to run repeatedly:
checkinput();

drive(speed_sp,ang_sp);
bool motorstatus1 = M1.checkrunning();

bool motorstatus2 = M2.checkrunning();

Serial.println("status"+String(motorstatus1)+","+String(motorstatus2));

////direction change
//drive(speed_sp,PI/2);
//delay(2000);
//drive(speed_sp,3*PI/2);
//delay(2000);
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

        
        if (readString.indexOf("v") > 0) {
          speed_sp = n/10 ;
          Serial.println("vel = " + String(speed_sp));
        }
        else if (readString.indexOf("s") > 0) {
          faststopon_all();
        }
        else if (readString.indexOf("o") > 0) {
          faststopoff_all();
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
