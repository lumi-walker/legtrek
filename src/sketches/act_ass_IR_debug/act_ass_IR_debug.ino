#include "Motor.h"
int accset = 4000;
int velset = 0;
String readString;
void setup() {
  // put your setup code here, to run once:
  analogWriteResolution(12);
  motor_init();//pinmode
  motor_ready();//on,holding,faststop
  M1.setdirect(0);
  M2.setdirect(1);
  Serial.begin(9600);
  while (! Serial);
  Serial.println("ready ");
}

void loop() {
  // put your main code here, to run repeatedly:
checkinput();
  analogWrite(M1_IN5,accset);
analogWrite(M1_IN6,velset);
//if (millis() < 5000){
//  analogWrite(M1_IN5,255);
//analogWrite(M1_IN6,0);
//}
//else if (millis() < 10000){
//analogWrite(M1_IN5,255);
//analogWrite(M1_IN6,100);
//}

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
          Serial.println("acc = " + String(accset));
        }
        else if (readString.indexOf("v") > 0) {
          velset = n ;
          Serial.println("vel = " + String(velset));
        }
        else if (readString.indexOf("fs") > 0) {
          faststopon_all();
        }
        else if (readString.indexOf("fso") > 0) {
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

