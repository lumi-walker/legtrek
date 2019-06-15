#include "BMS_pin_assignments.h"
#include <Arduino.h>
#include <Adafruit_MAX31855.h>
#include <SPI.h>
#include "TemperatureMonitor.h"
#include "RelayController.h"
// #include "ErrorLogger.h"
#define MAX_VOLTAGE_THRESHOLD 46
#define MIN_VOLTAGE_THRESHOLD 36
#define MAX_CURRENT_THRESHOLD 30
#define MAX_TEMPERATURE_THRESHOLD 60 //modify this


void init_BMS() {
	pinMode(PWR_PIN_1, OUTPUT);
	pinMode(PWR_PIN_2, OUTPUT);
/*
	pinMode(I_SENSE_READ_PIN_1, INPUT);
	pinMode(I_SENSE_READ_PIN_2, INPUT);
	pinMode(I_SENSE_READ_PIN_3, INPUT);
*/
	pinMode(T_SENSE_CHIP_SEL_1, OUTPUT);
	pinMode(T_SENSE_CHIP_SEL_2, OUTPUT);
	pinMode(T_SENSE_CHIP_SEL_3, OUTPUT);

	pinMode(V_SENSE_READ_PIN, INPUT);

	// pinMode(DUE2TEENSY_CALL_PIN, INPUT);
	// pinMode(DUE_RX_PIN, INPUT);
	// pinMode(TEENSY2DUE_CALL_PIN, OUTPUT);
	// pinMode(TEENSY_TX_PIN, OUTPUT);

	// SPI.begin();

}
