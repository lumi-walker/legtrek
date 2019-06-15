
#include "state_machine_utils.h"
//#include "voltage_monitor.h"
float temp;
std::vector<ErrorStatus> tempErrStatus;
float voltage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.begin();
  init_interrupts();
  currentState = stateDE;
  prevState = stateDE;
  LCD.loadingScreen();
  init_timer();
  init_motor();
//  init_BMS();
  LCD.startScreen();
//  relayController.connect();
  Serial.println("ready");
}

void loop() {
  //check for temperature and voltage
Serial.println("current mode is" + String(currentState));
//  if (temperatureMonitor.readTemperature(temp, tempErrStatus) == 0) {
//    for (int i = 0; i < tempErrStatus.size(); i++) {
//      if (tempErrStatus[i].errMsg == OVER_TEMPERATURE) {
//        is_OVER_TEMP = 1;
//        // go to decel state first
//        currentState = stateDecel;
//        // go to default after decel state
//        requestedState = stateCE;
//      }
//
//      if (tempErrStatus[i].errMsg == FAULTY_TEMPERATURE_SENSOR) {
//        // print error to lcd
//        is_FAULTY_TEMP_SENSOR = 1;
//      }
//    }
//  }
float ADCread = analogRead(V_SENSE_READ_PIN)*3.3/1024* 16;

//  is_LOW_VOLT = 1;
//  if (isMotorCorrect() == 0) {
//    is_MOTOR_ERROR = 1;
//    // go to decel state first
//    currentState = stateDecel;
//    // go to default after decel state
//    requestedState = stateCE;
//  }


//  LCD.writeBatteryLevel(voltage);

  LCD.writeErrorPanel();

  if (currentState != prevState) {
    LCD.writeMode(currentState);
    prevState = currentState;
  }
  if ((speed_sp  > prev_speed + 0.01 || speed_sp < prev_speed - 0.01)) { //write speed if speed_sp has updated;
    LCD.writeSpeed(speed_sp);
    prev_speed = speed_sp;
  }
  if (currTurn != prevTurn) {
    LCD.writeTurn(currTurn);
    prevTurn = currTurn;
  }


  switch (currentState) {
    case stateDE:
      //DEFAULT MODE
      runDefaultMode();
      break;
    case stateAA:
      //ACTIVE ASSIST MODE
      runActiveAssistMode();
      break;
    case stateJS:
      //JOYSTICK MODE
      runJoystickMode();
      break;
    case stateSS:
      //SETSPEED MODE
      runSetSpeedMode();
      break;
    case stateCE:
      //CRITICAL ERROR MODE
      runCriticalErrorMode();
      break;
    case stateTurnAA:
      //TURNING IN ACTIVE ASSIST
      runJoystickMode();
      break;
    case stateTurnSS:
      //TURNING IN SET SPEED
      runJoystickMode();
      break;
    case stateSit:
      //SITTING MODE
      runJoystickMode();
      break;
    case stateDecel:
      // DECEL MODE
      runDecelMode();
      break;
  }


}
