
/*
SMi21 Motor Configuration using DCmind Soft
The definitions are inverted due to inverters in circuit
expert program V101
IN1        On = 0
IN2        ----------TBD----------calibrate such that 1 is forward and 0 is backward
IN3        Low State Active; Holding Torque = 150
IN4        Low State Active
IN5        PWM; 0%:  ; 100%: ----------TBD----------
IN6        PWM; 0%: 2824 rpm; 100%: 0;          ------2824 rpm with wheel radius 6.25in, provides max speed 1.5mph
OUT1
OUT2
OUT3
OUT4

*/

#ifndef SMI21_H
#define SMI21_H

#include "motor_calib_cons.h"

class SMi21 {
    int onoffPin, direcPin,holdingPin,faststopPin,accPin,spdPin,muxPin,rspdPin,rdirecPin,isrunningPin,errPin;
  public:
    SMi21 (int,int,int,int,int,int,int,int,int,int,int);

    //functions
    void turnon();
    void turnoff();
    void faststopon();
    void faststopoff();
    void holdingon();
    void holdingoff();
    void setacc(int);
    void setspd(float);
    void setdirect(bool);
    bool checkrunning();
    float readspd();
    bool readdir();
};


SMi21::SMi21 (int in1, int in2,int in3, int in4,int in5, int in6, int mux, int out1, int out2, int out3, int out4) {
  onoffPin = in1;
  direcPin = in2;
  holdingPin = in3;
  faststopPin =  in4; //any digital pin
  accPin =  in5;//any analog pin
  spdPin =  in6;
  muxPin = mux;
  rspdPin = out1;
  rdirecPin = out2;
  isrunningPin = out3;
  errPin = out4;
}

void SMi21::turnon() {
  digitalWrite(onoffPin,HIGH);
}

void SMi21::turnoff() {
  digitalWrite(onoffPin,LOW);
}
void SMi21::faststopon(){
  digitalWrite(faststopPin,HIGH);
}
void SMi21::faststopoff(){
  digitalWrite(faststopPin,LOW);
}
void SMi21::holdingon(){
  digitalWrite(holdingPin,HIGH);
}
void SMi21::holdingoff(){
  digitalWrite(holdingPin,LOW);
}
void SMi21::setacc(int acc_rpmps){
  //------------------need to determine acc
  int acc_pwm = 4095-(acc_rpmps-min_acc_rpmps)/(max_acc_rpmps-min_acc_rpmps)*4095;
  if (acc_pwm < 0){
    analogWrite(accPin,0);
  }
  else if (acc_pwm > 4095){
    analogWrite(accPin,4095);
  }
  else{
    analogWrite(accPin,acc_pwm);
  }
}


void SMi21::setspd(float spd_mph){ //spd from UI
  //--------------------how to define direc
    if(spd_mph <= minSpeed) {
      spd_mph = minSpeed;
      digitalWrite(muxPin,GROUND);
      if (readspd()<=.1){
        faststopon();
      }
    }
    faststopoff();
    digitalWrite(muxPin,DAC);
    float maxspd_mph = maxSpeed;
    int spd_pwm = (spd_mph-minSpeed)/maxspd_mph*4095.0f;
    if (spd_pwm < 0){
      analogWrite(spdPin,0);
    }
    else if (spd_pwm > 4095){
      analogWrite(spdPin,4095);
    }
    else{
      analogWrite(spdPin,spd_pwm);
      Serial.println(spd_pwm);
    }

}

void SMi21::setdirect(bool direc){
    digitalWrite(direcPin,direc);
}

bool SMi21::checkrunning(){
  bool status = digitalRead(isrunningPin);
  return status;
}
float SMi21::readspd(){
  int readspeed_vol = analogRead(rspdPin);
  float readspeed_rpm = readspeed_vol*10; //------------calibrate!!!
  float readspeed_mph = readspeed_rpm/1859;
}
bool SMi21::readdir(){
  int realdir = digitalRead(rdirecPin);
}


#endif
