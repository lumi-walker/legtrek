#ifndef VOLTAGE_MONITOR_H
#define VOLTAGE_MONITOR_H

#include "BMS_pin_assignments.h"
#include "BMSUtils.h"
#include <Arduino.h>

#define OVER_VOLTAGE_THRESHOLD 42f;
#define UNDER_VOLTAGE_THRESHOLD 34f;

#define VOLT_PIN = V_SENSE_READ_PIN;

static const BMSErrorCode VoltageErrorCode[3] = {LOW_VOLTAGE_ERROR,
												OVER_VOLTAGE_ERROR, NO_ERROR};

class VoltageMonitor {
	public:

	VoltageMonitor(){

	}

	~VoltageMonitor(){

	}

	//perc_voltage gives you what percent the battery is at
	//update tells you when battery has gone down by 5%
	bool readVoltage(float& perc_voltage, ErrorStatus& errorStat, bool& update) {
		bool SUCCESS = true;

		float ADCread = analogRead(VOLT_PIN);
		float voltRead = ADCread*3.3/1024;

		voltage = voltRead * 16;

		float min_volt = 34;
		float max_volt = 42;

		ErrorStatus v;

		if (voltage > OVER_VOLTAGE_THRESHOLD) {
			SUCCESS = false;
			v.errType = VOLTAGE_ERROR;
			v.errCode = OVER_VOLTAGE_ERROR;
			v.errMsg = OVER_VOLTAGE;
		} else if (voltage < UNDER_VOLTAGE_THRESHOLD) {
			SUCCESS = false;
			v.errType = VOLTAGE_ERROR;
			v.errCode = LOW_VOLTAGE_ERROR;
			v.errMsg = LOW_VOLTAGE;
		} else {
			//v.errType = VOLTAGE_ERROR;
			//v.errCode = NO_ERROR;

			perc_voltage = 100 * (voltage - min_volt)/(max_volt-min_volt);

			if (prev_perc == -1)
			{
				prev_perc = perc_voltage;
				update = false;
			}
			else if (abs(perc_voltage - prev_perc) >= 5) {
				prev_perc = perc_voltage;
				update = true;
			}
			else {
				update = false;
			}
		}

    return SUCCESS;
	}

	private:

		float prev_prec = -1;

};
#endif
