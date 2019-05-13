#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

#include "BMS_pin_assignments.h"
#include <Arduino.h>
#include<TimerOne.h>

#define PULSE_DURATION_MICROS 250000



class RelayController {
public:
	RelayController(uint8_t pin1, uint8_t pin2) {
		A1 = pin1;
		A2 = pin2;
		pinMode(A1,OUTPUT);
		pinMode(A2,OUTPUT);
	}

	void connect(){

	  	digitalWrite(A1,LOW);
	  	digitalWrite(A2,HIGH);
		
		prevT = micros();
		while(micros() - prevT < PULSE_DURATION_MICROS);

		neutralize();

	}

	void disconnect() {
  
	  
	  	digitalWrite(A1,HIGH);
	  	digitalWrite(A2,LOW);
		
		prevT = micros();
		while(micros() - prevT < PULSE_DURATION_MICROS);

		neutralize();

		
	}

	void neutralize() {
		digitalWrite(A1,LOW);
	    digitalWrite(A2,LOW);
	}

private:
	long prevT;

	uint8_t A1;
	uint8_t A2;
};

#endif