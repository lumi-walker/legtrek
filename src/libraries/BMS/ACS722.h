#ifndef ACS722_H
#define ACS722_H

#include "BMS_pin_assignments.h"


#define ACS722_SENSITIVITY 0.066f // 66mV/A
#define ACS722_ZERO_CURRENT_OUTPUT_VOLTAGE 0.33f


class ACS722 {
public:
	ACS722(const uint8_t& pin) {
		readPin = pin;
	}

	~ACS722() {

	}

	float readCurrent() {
		uint16_t val = analogRead(readPin);
		// convert bits to voltage and subtract offset
		float voltage = 3.3 * float(val)/1023.0 - ACS722_ZERO_CURRENT_OUTPUT_VOLTAGE;
		if(voltage < 0 ) voltage = 0;
		
		return voltage / ACS722_SENSITIVITY;

	}

private:
	uint8_t readPin;
};

#endif