#include "motor_pin_assignments.h"
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



float DEFAULT_ACCEL = 200.0f;
bool FORWARD = true;
bool REVERSE = false;

class SMi21 {

    int onoffPin, direcPin,holdingPin,faststopPin,accPin,velPin;
    bool fastStopOn;
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
    void decelerate(float,float);
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
  fastStopOn = true;
  digitalWrite(faststopPin,HIGH);
}
void SMi21::faststopoff(){
  fastStopOn = false;
  digitalWrite(faststopPin,LOW);
}
void SMi21::holdingon(){
  digitalWrite(holdingPin,HIGH);
}
void SMi21::holdingoff(){
  digitalWrite(holdingPin,LOW);
}
void SMi21::setacc(int acc){ //vel from UI
  //------------------need to determine acc
  int acc_pwm = 4095-acc;
  analogWrite(accPin,acc_pwm);
}


void SMi21::setvel(float vel_mph){ //vel from UI
  //--------------------how to define direc
    faststopoff();
    
    if(vel_mph <= 0.3) {
      vel_mph = 0.3;  
    }

    float maxvel_mph = 1.5;
    int vel_pwm = (vel_mph-.3)/maxvel_mph*4095.0f;
    analogWrite(velPin,vel_pwm);
}

void SMi21::setdirect(bool direc){
    digitalWrite(direcPin,direc);
}

void SMi21::decelerate(float vel_mph,float acc){
  if (vel_mph <= .3){
    faststopon();
  }
  else{
    setvel(0);
    //calc waitTime after determine scaling for accleration
    long waittime = 1000;
    delay(waittime);
    faststopon();
  }
}

