#ifndef TEMPERATURE_MONITOR_H
#define TEMPERATURE_MONITOR_H



#include <Adafruit_MAX31855.h>
#include "BMS_pin_assignments.h"
#include <vector>
#include "BMSUtils.h"
#include <Arduino.h>

#define TEMPERATURE_MONITOR_REDUNDANCY_CHECK_THRESHOLD 0.15f
#define OVER_TEMPERATURE_THRESHOLD	50.f // degrees C (max handle is 60)
static const BMSErrorCode TemperatureErrorCode[5] = {OVER_TEMPERATURE_ERROR, 
								   					OPEN_CIRCUIT_ERROR, 
								   					SHORT_TO_GND_ERROR, 
								   					SHORT_TO_VCC_ERROR, 
								   					SHORT_TO_VCC_AND_SHORT_TO_GND_ERROR};



class TemperatureMonitor {
public:

	TemperatureMonitor() {

	}

	~TemperatureMonitor() {
		
	}

	bool readTemperature(float& temp, std::vector<ErrorStatus>& errStatus) {
		bool SUCCESS = true;
		float tmp[3] = {0, 0 , 0};

		uint8_t errCode;
		if(!errStatus.empty()) {
			errStatus.clear();
		}

		ErrorStatus t;

		for(int i = 0; i < ALL_SENSORS; i++) {
			tempReadings[i] = tSense[i].readCelsius();
			if(isnan(tempReadings[i])) {
				//error occured!
				
				SUCCESS = false;
				errCode = tSense[i].readError();
				for(BMSErrorCode err : TemperatureErrorCode) {
					if((BMSErrorCode)errCode == err) {
						t.errType = TEMPERATURE_ERROR;
						t.errCode = err;
						t.faultySensorIndex = (SensorIndex)i;

						if(err == OVER_TEMPERATURE_ERROR) {
							t.errMsg = OVER_TEMPERATURE;
						} else {
							t.errMsg = FAULTY_TEMPERATURE_SENSOR;
						}
						errStatus.push_back(t);
						break;
					}
				}
			} else{
				tmp[i] = tempReadings[i];
			}
		}
		SensorIndex faultySensorIndex;

		bool isErrors = checkForFaultySensor(&tempReadings[0],faultySensorIndex,TEMPERATURE_MONITOR_REDUNDANCY_CHECK_THRESHOLD);

		if(isErrors) {
			SUCCESS = false;
			t.errType = TEMPERATURE_ERROR;
			t.errCode = FAULTY_SENSOR_ERROR;
			t.faultySensorIndex = faultySensorIndex;
			t.errMsg = FAULTY_TEMPERATURE_SENSOR;
			errStatus.push_back(t);

			if(faultySensorIndex != ALL_SENSORS) {
				tmp[faultySensorIndex] = 0;

				for(int i = 0; i < ALL_SENSORS; i++) {
					temp += tmp[i];
				}
				temp /= 2;

			} else {
				// all unreliable... take the max temp reading

				float maxtemp = tmp[0];
				for(int i = 0; i < ALL_SENSORS; i++) {
					if(tmp[i] > maxtemp) {
						maxtemp = tmp[i];
					}
				}
				temp = maxtemp;
			}
			
		} else {
			// take average of all measurements as you read them
			temp = (tmp[0] + tmp[1] + tmp[2])/3; 
		}

		if(temp > OVER_TEMPERATURE_THRESHOLD) {
			SUCCESS = false;
			t.errType = TEMPERATURE_ERROR;
			t.errCode = OVER_TEMPERATURE_ERROR;
			t.errMsg = OVER_TEMPERATURE;
			t.faultySensorIndex = ALL_SENSORS;
			errStatus.push_back	(t);
		}
		

/*
			Serial.println("_____________________________________________________________________________________");
			Serial.println("TEMPERATURE READINGS");
			Serial.println("_____________________________________________________________________________________");
			Serial.println();
		for(int i = 0; i < 3; i++) {
			Serial.print("Sensor " + String(i) + " : " + String(tempReadings[i]));
			if(i != 2) {
				Serial.print(" | ");
			} else {
				Serial.println(" | Estimated Temp: " + String(temp));
				if(!errStatus.empty()) {
					Serial.println("TEMPERATURE READINGS ERROR");

					for(int i = 0; i < errStatus.size(); i++) {
        				Serial.println("SENSOR : " + String(errStatus[i].faultySensorIndex) + " | ERROR CODE : " + String(errStatus[i].errCode));
   					 }		
				}
				Serial.println("_____________________________________________________________________________________");
				Serial.println();
			}
		}
		*/
		return SUCCESS;
	}

private:

	Adafruit_MAX31855 tSense[ALL_SENSORS] = {Adafruit_MAX31855(TEENSY_CLK_PIN, T_SENSE_CHIP_SEL_1, TEENSY_MISO_PIN),
											 Adafruit_MAX31855(TEENSY_CLK_PIN, T_SENSE_CHIP_SEL_2, TEENSY_MISO_PIN),
											 Adafruit_MAX31855(TEENSY_CLK_PIN, T_SENSE_CHIP_SEL_3, TEENSY_MISO_PIN)};

	float tempReadings[ALL_SENSORS];

};


#endif