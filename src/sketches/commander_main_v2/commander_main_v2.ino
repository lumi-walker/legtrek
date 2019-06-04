#include "LCD.h"
#include "state_machine_utils.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.begin();
  init_interrupts();
  currentState = stateDE;
  prevState = stateDE;
  LCD.loadingScreen();
  init_timer();
  motor_init();
  motor_ready();
  LCD.startScreen();
  Serial.println("ready");
}

void loop() {
    Serial.println(currentState);
    if (currentState != prevState) {
      LCD.writeMode(currentState);
      prevState = currentState;
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
  
    if ((speed_sp  > prev_speed + 0.01 || speed_sp < prev_speed - 0.01)) { //write speed if speed_sp has updated;
      LCD.writeSpeed(speed_sp);
      prev_speed = speed_sp;
    }
    if (currTurn != prevTurn) {
      LCD.writeTurn(currTurn);
      prevTurn = currTurn;
    }
}
