

#include "BMS.h"


/*
 * Due/Teensy Error Communication Protocol:
 * 
 * The Teensy will ping the Due via a digital pin. 
 * The Teensy will then send a byte of data to the Due via UART in the following format:
 * 
 * 1) the most significant bit (bit 6) will be 1 for an error message and 0 for a voltage message
 *    ERROR_MESSAGE_MASK is used to tag a message as an error message on the Teensy's end.
 *    For example:
 *      msg = 0b00000000 (0 in binary)
 *      msg | ERROR_MESSAGE_MASK = 0b00000000 | 0b10000000 = 0b10000000 (bitwise logical OR operation)
 *      
 * 2) The Due is expected to notify user of shutdown due to critical error. It will know if there is a critical error if:
 *    msg | CRITICAL_ERROR_MASK = 1
 */
byte ERROR_MESSAGE_MASK = (1 << 7);
byte CRITICAL_ERROR_MASK = (1 << OVER_TEMPERATURE) | (1 << OVER_VOLTAGE) | (1 << LOW_VOLTAGE);

// BMSError = true when there is a problem with Voltage Monitor or Temperature Monitor
bool BMSError = false;

/*
 * OKAY_TO_UNPLUG is a special message sent to Due via Serial1 to indicate
 * to the user that it's okay to unplug the battery
 */
#define OKAY_TO_UNPLUG  0xFF


//DUE_REPLIED is a flag used to indicate when the Due is done turning off the motors
// in the event of a critical error (OVER TEMP, OVER VOLTAGE, LOW VOLTAGE)
volatile bool DUE_REPLIED = false;

// Interrupt Service Routine : function that is called when the DUE2TEENSY_CALL_PIN goes from LOW -> HIGH
void DueReplied() {
  DUE_REPLIED = true;
}



TemperatureMonitor temperatureMonitor;
RelayController relay(PWR_PIN_1, PWR_PIN_2);
ErrorLogger errorLogger;

void setup() {
  // initialize serial monitor
  Serial.begin(9600);

  // initialize UART TX/RX on Teensy
  Serial1.begin(115200);

  // Serial1 has multiple options for TX and RX pins, choose the set
  Serial1.setTX(26);
  Serial1.setRX(27);

  attachInterrupt(digitalPinToInterrupt(DUE2TEENSY_CALL_PIN),DueReplied,FALLING);
  interrupts();
  delay(500);

}

// prevErrorMessage is used to avoid sending duplicate error messages
byte prevErrorMessage = 0;

void loop() {
  // variable storing the error message
  byte errorMessage = 0;

  // variables for temperature and voltage readings
  float temp;
  float voltage;


  std::vector<ErrorStatus> tempErrStatus;
  std::vector<ErrorStatus> errors2send;
  
  // read temperature
  bool SUCCESS = temperatureMonitor.readTemperature(temp,tempErrStatus);

  if(!tempErrStatus.empty()) {
    errorMessage |= ERROR_MESSAGE_MASK;
    for(int i = 0; i < tempErrStatus.size(); i++) {
      errors2send.push_back(tempErrStatus[i]);
      errorMessage |= (1<< tempErrStatus[i].errMsg);
    }
  }

   /*  
    *   READ VOLTAGE
    *   
    *   SUCCESS &= voltageMonitor.readVoltage(voltage, voltageErrStatus);
    *   if not success then append errorStatus to errors2send and update errorMessage
    *   if voltage drops 5%, ping Due and send voltage percentage
    */
    
   if(errorMessage != prevErrorMessage) {
      // different error message, ping Due

      // trigger interrupt on Due
       digitalWrite(TEENSY2DUE_CALL_PIN,HIGH);
       delay(5);
       digitalWrite(TEENSY2DUE_CALL_PIN,LOW);

       // write message to Due via UART 
       Serial1.write(errorMessage);

       prevErrorMessage = errorMessage;
   } else {
      // do not ping Due  
   }
   
  if(!SUCCESS) {

      
       if(errorMessage & CRITICAL_ERROR_MASK) {
       // critical error... Due warns the user that shutdown is required
       // wait for Due to turn off motors and respond
          while(!DUE_REPLIED);

       // disconnect motors and lift columns
          relay.disconnect();
          
       // log errors  
          errorLogger.log(errors2send); // critical log

       // tell Due that it can display to user that they can unplug the battery now
          digitalWrite(TEENSY2DUE_CALL_PIN,HIGH);
          delay(5);
          digitalWrite(TEENSY2DUE_CALL_PIN,LOW);
          Serial1.write(OKAY_TO_UNPLUG);
          
       } else {
       // log the errors 
          errorLogger.log(errors2send); // not critical log (debug log)
       }  
  }
     
}

  
  
  
  
