#ifndef BMS_UTILS_H
#define BMS_UTILS_H

#include <string>
#include <Arduino.h>
#include <map>

enum {
	SENSOR_1,
	SENSOR_2,
	SENSOR_3,
	ALL_SENSORS
} typedef SensorIndex;


enum {
	TEMPERATURE_ERROR,
//	CURRENT_ERROR, OBSELETE
	VOLTAGE_ERROR
} typedef BMSErrorType;

// BMS system will use this to output verbose error log to SD card
enum {
	OVER_TEMPERATURE_ERROR = 0,
	OPEN_CIRCUIT_ERROR = 1,
	SHORT_TO_GND_ERROR = 2,
	SHORT_TO_VCC_ERROR = 4,
	SHORT_TO_VCC_AND_SHORT_TO_GND_ERROR = 6,
//	OVER_CURRENT_ERROR = 7, OBSELETE
//	OVER_CURRENT_AND_FAULTY_SENSOR = 8, OBSELETE
	FAULTY_SENSOR_ERROR = 9,
	LOW_VOLTAGE_ERROR = 10,
	OVER_VOLTAGE_ERROR = 11,
	NO_ERROR
} typedef BMSErrorCode;


//  commander system will use these to display message
enum {
	FAULTY_TEMPERATURE_SENSOR,
	OVER_TEMPERATURE, // below here is critical error
	OVER_CURRENT,
	OVER_VOLTAGE,
	LOW_VOLTAGE,
	NUM_MESSAGES
} typedef BMSErrorMessage;


struct ErrorStatus {
	BMSErrorType errType;
	SensorIndex faultySensorIndex;
	BMSErrorCode errCode;
	BMSErrorMessage errMsg;
};

float calcPercentDifference(float val1, float val2) {
	float tmp;
	if(val1 > val2) {
		tmp = val1 - val2;
	} else {
		tmp = val2 - val1;
	}
	return tmp/((val1 + val2)/2.0);
}


bool checkForFaultySensor(float measurements[3], SensorIndex& faultySensorIndex, float REDUNDANCY_CHECK_THRESHOLD) {
	bool isErrors = false;

	/*
		4 possible outcomes: sensor 1 faulty, sensor 2 faulty, sensor 3 faulty, or none agrees
	*/
	float percentErr12;
	float percentErr13;
	float percentErr23;

	percentErr12 = calcPercentDifference(measurements[SENSOR_1],measurements[SENSOR_2]);
	percentErr13 = calcPercentDifference(measurements[SENSOR_1],measurements[SENSOR_3]);
	percentErr23 = calcPercentDifference(measurements[SENSOR_2],measurements[SENSOR_3]);


	#ifdef _PRINT_PERCENT_ERROR
		Serial.println("Err12 : " + String(percentErr12) + " // Err13 : " + String(percentErr13) + " // Err23 : " + String(percentErr23));
	#endif

	if(percentErr12 > REDUNDANCY_CHECK_THRESHOLD) {
		// error between 1 and 2
		// is 1 or 2 the correct val?
		isErrors = true;
		if(percentErr13 > REDUNDANCY_CHECK_THRESHOLD) {
			//error between 1 and 3
			//is 1 faulty or all faulty?
			if(percentErr23 > REDUNDANCY_CHECK_THRESHOLD) {
				// all faulty because 1,2 and 3 all disagree
				faultySensorIndex = ALL_SENSORS;
			} else {
				//1 is faulty because 2 and 3 agree
				faultySensorIndex = SENSOR_1;
			}
		} else {
			//2 is faulty because 1 and 3 agree
			faultySensorIndex = SENSOR_2;
		}
	} else {
		//1 and 2 agree
		//does 3 agree?
		if(percentErr23 > REDUNDANCY_CHECK_THRESHOLD) {
			// 2 and 3 disagree
			// 3 is faulty
			isErrors = true;
			faultySensorIndex = SENSOR_3;
		} else {
			// all agree
			isErrors = false;
		}
	}

	return isErrors;
}

#endif