//State Machine Logic
#include "ui_functions_init.h"

void DefaultMode() {
  setSpeed = 0.00;
}

void ActiveAssistMode() {
  buttonBlink(lAA);
}

void JoystickMode() {
  readJoystick();
  buttonBlink(lJS);
}

void SetSpeedMode() {
  buttonBlink(lSS);
}

void TurningMode() {
  buttonBlink(lTN);
}

void CriticalErrorMode(){
  setSpeed = 0.0;
}

void SittingMode() {

}

void buttonBlink(int whichLED) {
  if (millis() % 2000 < 1000) {
    digitalWrite(whichLED,HIGH);
  }
  else {
    digitalWrite(whichLED,LOW);
  }
}


void ShutDownProcedure(ShutDown stage) {
  switch(stage) {
    case TEENSY_TO_DUE:
      stage = DUE_TURN_OFF_MOTOR;
      break;
    case DUE_TURN_OFF_MOTOR:
      stage = DUE_TO_TEENSY;
      break;
    case DUE_TO_TEENSY:
      stage = TEENSY_TO_DUE2;
      break;
    case TEENSY_TO_DUE2:
      stage = DUE_PRINT_LCD;
      break;
    case DUE_PRINT_LCD:
        //print to LCD
      break;
    default:
      break;
  }
}

void runStateMachine(State currState) {
  if (currState != prevState) { //only change LCD if the state is changed
    LCD.writeMode(currState);
    prevState = currState;
  }
  switch(currState) {
    case stateDE:
      DefaultMode();
      break;
    case stateAA:
      ActiveAssistMode();
      break;
    case stateJS:
      JoystickMode();
      break;
    case stateSS:
      SetSpeedMode();
      break;
    case stateCE:
      CriticalErrorMode();
      break;
    case stateTurnAA:
      TurningMode();
      break;
    case stateTurnSS:
      TurningMode();
      break;
    case stateSit:
      SittingMode();
      break;
    default:
      DefaultMode();
      break;
  }
}
