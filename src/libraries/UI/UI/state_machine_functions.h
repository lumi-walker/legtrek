#ifndef STATE_MACHINE_FUNCTIONS_H
#define STATE_MACHINE_FUNCTIONS_H
#include "ui_functions_init.h"
#include "Motor.h"

// UI function prototypes, definitions below


void runDefaultMode() {
}

void runActiveAssistMode() {
  buttonBlink(lAA);
  if(initStride) {
    vel_sp = AA_vel_sp;
    setspeed(vel_sp,FORWARD);
    initStride = false;
  } else {
    if(micros() - strideStartTime > strideStepTime) {
      decel_to_zero(vel_sp);
    }
  }
}

void runJoystickMode() {
  buttonBlink(lJS);
  readJoystick();

  if(rRead > rDeadBand) {
    if(angRead < validAngleRangeForward_max && angRead > validAngleRangeForward_min) {
      // move forward
       stateRTN = false;
       stateLTN = false;
       vel_sp = (rRead - rDeadBand)*(maxSpeed - minSpeed) + minSpeed;
       setspeed(vel_sp,FORWARD);
       Serial.println("FORWARD");

    } else if(angRead < validAngleRangeRTurn_max && angRead > validAngleRangeRTurn_min) {
        //right turn
        // bring speed to 0 if not turning before

        if(stateRTN == false) {
          decel_to_zero(vel_sp);
          stateRTN = true;
          stateLTN = false;
          vel_sp = turn_vel_sp;
          turnRight(vel_sp);
        }
    } else if(angRead < validAngleRangeLTurn_max && angRead > validAngleRangeLTurn_min) {
      // left turn

        if(stateLTN == false) {
          decel_to_zero(vel_sp);
          stateRTN = false;
          stateLTN = true;
          vel_sp = turn_vel_sp;
          turnLeft(vel_sp);
        }
    }
  } else {
    // radius not outside deadband

    stateRTN = false;
    stateLTN = false;
    decel_to_zero(vel_sp);
  }

}

void runSetSpeedMode() {
  buttonBlink(lSS);
  if(vel_sp != prev_vel_sp) {
    setspeed(vel_sp,FORWARD);
    prev_vel_sp = vel_sp;
  }
}

void runTurningMode() {
  buttonBlink(lTN);
  readJoystick();
  if(rRead > rDeadBand) {
    if(angRead < validAngleRangeRTurn_max && angRead > validAngleRangeRTurn_min) {
      // turn right
      Serial.println("TURN RIGHT");
          if(stateRTN == false) {
            decel_to_zero(vel_sp);
            stateRTN = true;
            stateLTN = false;
            vel_sp = turn_vel_sp;
            turnRight(vel_sp);
          }
    } else if(angRead < validAngleRangeLTurn_max && angRead > validAngleRangeLTurn_min) {
      // turn left
      Serial.println("TURN LEFT");
          if(stateLTN == false) {
            decel_to_zero(vel_sp);
            stateRTN = false;
            stateLTN = true;
            vel_sp = turn_vel_sp;
            turnLeft(vel_sp);
          }
    } else {
          stateRTN = false;
          stateLTN = false;
          decel_to_zero(vel_sp);
    }
  } else {
          stateRTN = false;
          stateLTN = false;
          decel_to_zero(vel_sp);
  }
}

void runCriticalErrorMode(){
  vel_sp = 0.0;
}

void runSittingMode() {

}




#endif
