#ifndef MOTOR_H
#define MOTOR_H

#include "SMi21.h"
#include "motor_pin_assignments.h"


void motor_init() {
	pinMode(M1_IN1, OUTPUT);
	pinMode(M1_IN2, OUTPUT);
	pinMode(M1_IN3, OUTPUT);
	pinMode(M1_IN4, OUTPUT);
  pinMode(M1_IN5, OUTPUT);
  pinMode(M1_IN6, OUTPUT);

  pinMode(M1_OUT1, INPUT);
	pinMode(M1_OUT2, INPUT);
	pinMode(M1_OUT3, INPUT);
	pinMode(M1_OUT4, INPUT);

  pinMode(M2_IN1, OUTPUT);
	pinMode(M2_IN2, OUTPUT);
	pinMode(M2_IN3, OUTPUT);
	pinMode(M2_IN4, OUTPUT);
  pinMode(M2_IN5, OUTPUT);
  pinMode(M2_IN6, OUTPUT);

  pinMode(M2_OUT1, INPUT);
  pinMode(M2_OUT2, INPUT);
  pinMode(M2_OUT3, INPUT);
  pinMode(M2_OUT4, INPUT);

	pinMode(M1_MUX,OUTPUT);
	pinMode(M2_MUX,OUTPUT);
}

SMi21 M1(M1_IN1,M1_IN2,M1_IN3,M1_IN4,M1_IN5,M1_IN6,M1_MUX,M1_OUT1,M1_OUT2,M1_OUT3,M1_OUT4);
SMi21 M2(M2_IN1,M2_IN2,M2_IN3,M2_IN4,M2_IN5,M2_IN6,M2_MUX,M2_OUT1,M2_OUT2,M2_OUT3,M2_OUT4);

void motor_ready(){
	analogWriteResolution(12);
	M1.turnon();
  M2.turnon();

  M1.faststopoff();
  M1.holdingoff();

  M2.faststopoff();
  M2.holdingoff();

	M1.setacc(DEFAULT_ACCEL);
}

void drive(float speed, double angle){
	//expected input angle: 0,PI,PI/2,(15-75 degree),(105-165 degree)
	M1.setspd(speed);
  M2.setspd(speed);
	bool M1direct=FORWARD;
	bool M2direct=FORWARD;

	if (angle == 0){//right turn in place
		bool M1direct=FORWARD;
		bool M2direct=REVERSE;

	}else if(angle == PI){//left turn in place
		bool M1direct=REVERSE;
		bool M2direct=FORWARD;
	}else{//turn in arc
		if(angle <= 75*PI/180){
			M1.setspd(speed);
			float slowerspeed = (angle-(15*PI/180))/(60*PI/180)*speed; //linear mapping
			M2.setspd(slowerspeed);
		}else{
			M2.setspd(speed);
			float slowerspeed = speed - (angle-(105*PI/180))/(60*PI/180)*speed; //linear mapping
			M1.setspd(slowerspeed);
		}
	}
	M1.setdirect(M1direct);
	M2.setdirect(M2direct);
	//make sure to check with hardware that readdir works when stopped/edge casess
	if (M1.readdir()!=M1direct || M2.readdir()!=M2direct){
		M1.setspd(0);
	  M2.setspd(0);
	}

}
void setacc_all(int acc){
	M1.setacc(acc);
	M2.setacc(acc);
}

void faststopon_all(){
	M1.faststopon();
	M2.faststopon();
}

void faststopoff_all(){
	M1.faststopoff();
	M2.faststopoff();
}

bool isMotorRunning(){
	bool running = true;
	if (M1.checkrunning()==false && M2.checkrunning()==false){
		bool running = false;
	}
	return running;
}


#endif
