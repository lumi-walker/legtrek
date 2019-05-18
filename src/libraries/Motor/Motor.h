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

SMi21 M1(M1_IN1,M1_IN2,M1_IN3,M1_IN4,M1_IN5,M1_IN6,M1_MUX);
SMi21 M2(M2_IN1,M2_IN2,M2_IN3,M2_IN4,M2_IN5,M2_IN6,M2_MUX);
//,M1_OUT1,M1_OUT2,M1_OUT3,M1_OUT4

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



void turnRight(float vel_sp){
	M1.setvel(vel_sp);
	M2.setvel(vel_sp);
	M1.setdirect(1);
	M2.setdirect(1);
}

void turnLeft(float vel_sp){
	M1.setvel(vel_sp);
	M2.setvel(vel_sp);
	M1.setdirect(0);
	M2.setdirect(0);
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

void decel_to_zero(float vel_sp){
	float acc=DEFAULT_ACCEL;
	if(vel_sp > minSpeed) {

		setacc_all(acc);
		M1.setvel(minSpeed);
		M2.setvel(minSpeed);

		float vel_rpm = 1882.35*vel_sp;
		long waitTime = vel_rpm/acc*1000;
		vel_sp = minSpeed;
		delay(waitTime);
		M1.faststopon();
		M2.faststopon();
		Serial.println(acc);
		Serial.println(vel_sp);
		Serial.println(waitTime);
		Serial.println("DECELL COMPLETE");
	} else {
		M1.faststopon();
		M2.faststopon();
	}
}

void setspeed(float speed,bool direc){

  M1.setvel(speed);
  M2.setvel(speed);

  if(speed == 0) {
  	faststopon_all();
  }
	M1.setdirect(!direc);
	M2.setdirect(direc);
}

#endif
