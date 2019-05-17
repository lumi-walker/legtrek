
/*
SMi21 Motor Configuration using DCmind Soft
The definitions are inverted due to inverters in circuit
expert program V101
IN1        On = 0
IN2        ----------TBD----------
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

#include "global_include.h"
float DEFAULT_ACCEL = 500.0f;
bool FORWARD = true;
bool REVERSE = false;
int max_acc_rpmps = 956;
int min_acc_rpmps = 483;

class SMi21 {
    int onoffPin, direcPin,holdingPin,faststopPin,accPin,velPin;
  public:
    SMi21 (int,int,int,int,int,int);

    //functions
    void turnon();
    void turnoff();
    void faststopon();
    void faststopoff();
    void holdingon();
    void holdingoff();
    void setacc(int);
    void setvel(float);
    void setdirect(bool);
};


SMi21::SMi21 (int in1, int in2,int in3, int in4,int in5, int in6) {
  //, int out1, int out2, int out3, int out4
  onoffPin = in1;
  direcPin = in2;
  holdingPin = in3;
  faststopPin =  in4; //any digital pin
  accPin =  in5;//any analog pin
  velPin =  in6;
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
void SMi21::setacc(int acc_rpmps){ //vel from UI
  //------------------need to determine acc
  int acc_pwm = 4095-(acc_rpmps-min_acc_rpmps)/(max_acc_rpmps-min_acc_rpmps)*4095;
  analogWrite(accPin,acc_pwm);
}


void SMi21::setvel(float vel_mph){ //vel from UI
  //--------------------how to define direc
    if(vel_mph <= minSpeed) {
      vel_mph = minSpeed;
      faststopon();
    }
    faststopoff();
    float maxvel_mph = maxSpeed;
    int vel_pwm = (vel_mph-minSpeed)/maxvel_mph*4095.0f;
    if (vel_pwm < 0){
      analogWrite(velPin,0);
    }
    else if (vel_pwm > 4095){
      analogWrite(velPin,4095);
    }
    else{
      analogWrite(velPin,vel_pwm);
      Serial.println(vel_pwm);
    }

}

void SMi21::setdirect(bool direc){
    digitalWrite(direcPin,direc);
}

#endif
