#ifndef ERROR_LOGGER_H
#define ERROR_LOGGER_H

#include<string>
#include<Arduino.h>
#include "BMSUtils.h"
#include <SD.h>
#include <map>

const int SD_CARD_CS = BUILTIN_SDCARD;

namespace std {
  void __throw_bad_alloc()
  {
    Serial.println("Unable to allocate memory");
    return;
  }

  void __throw_length_error( char const*e )
  {
    Serial.print("Length Error :");
    Serial.println(e);
    return;
  }
}




class ErrorLogger {
public:

	ErrorLogger() {
		// init file name to "log-YY/MM/DD-index.txt", check if file name exists first
		// initialize time
		init();
	}

	~ErrorLogger() {

	}


	void log(std::vector<ErrorStatus> errors) {
		SD.begin(SD_CARD_CS);

		errorLog = SD.open("critical-errors-log.txt",FILE_WRITE);

		String line;
		for(int i = 0; i < errors.size(); i++) {
			line = formLine(errors[i]);
			if(errorLog) {
				Serial.println("Writing: " + line);
				errorLog.println(line);
			}
		}
		
		errorLog.close();
	}





private:
	void init() {
		SD.begin(SD_CARD_CS);
		errorLog = SD.open("fileNameArchive.txt",FILE_READ);

		if(!errorLog) {
			errorLog = SD.open("fileNameArchive.txt",FILE_WRITE);
			errorLog.println("0");
		} else {
			errorLog = SD.open("fileNameArchive.txt",FILE_READ);
			// grab latest file
		}
	}

	String code2String(BMSErrorCode errCode) {
		switch(errCode) {
			case OVER_TEMPERATURE_ERROR:
				return "CODE : OVER_TEMPERATURE";
				break;
			case OPEN_CIRCUIT_ERROR:
				return "CODE : OPEN CIRCUIT ERROR";
				break;
			case SHORT_TO_GND_ERROR:
				return "CODE : SHORT_TO_GND_ERROR";
				break;
			case SHORT_TO_VCC_ERROR:
				return "CODE : SHORT_TO_VCC_ERROR";
				break;
			case SHORT_TO_VCC_AND_SHORT_TO_GND_ERROR:
				return "CODE : SHORT_TO_VCC_AND_SHORT_TO_GND_ERROR";
				break;
				/*
			case OVER_CURRENT_ERROR:
				return "CODE : OVER_CURRENT_ERROR";
				break;

			case OVER_CURRENT_AND_FAULTY_SENSOR:
				return "CODE : OVER_CURRENT_AND_FAULTY_SENSOR";
				break;
				*/
			case FAULTY_SENSOR_ERROR:
				return "CODE : FAULTY_SENSOR_ERROR";
				break;
			case LOW_VOLTAGE_ERROR:
				return "CODE : LOW_VOLTAGE_ERROR";
				break;
			case OVER_VOLTAGE_ERROR:
				return "CODE : OVER_VOLTAGE_ERROR";
				break;
			case NO_ERROR:
				return "CODE : NO_ERROR";
				break;

		};
	}

	String type2String(BMSErrorType type) {
		if(type == TEMPERATURE_ERROR) {
			return "ERROR TYPE : TEMPERATURE ERROR";
		}

		if(type == VOLTAGE_ERROR) {
			return "ERROR TYPE : VOLTAGE ERROR";
		}

		return "";
	}

	String formLine(ErrorStatus errStatus) {
		String line;
		line = type2String(errStatus.errType) + " | ";
		line = line + code2String(errStatus.errCode) + " | ";
		if(errStatus.faultySensorIndex == ALL_SENSORS) {
			line = line + "SENSOR INDEX : ALL SENSORS";
		} else {
			line = line + "SENSOR INDEX : " + String((int)errStatus.faultySensorIndex + 1);
		}
		return line;
	}
	

	// store time since file opened
	long prevT;
	long minutes = 0;
	long hours = 0;

	File errorLog;
	std::string defaultFileName = "ERROR-LOG-";
	std::string fileName;
};


#endif