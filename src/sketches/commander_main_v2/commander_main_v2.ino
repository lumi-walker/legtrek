
#include "UI_utils.h"
#include "ui_pin_assignments.h"
#include "motor_calib_cons.h"

// state-machine typedef
enum {
  stateDE, //Default mode
  stateAA, //Active Assist mode
  stateJS, //Joystick mode
  stateSS, //Set Speed mode
  stateCE, //Crtical Error mode
  stateTurnAA, //Turning in AA
  stateTurnSS, //Turning in SS
  stateSit, //Sitting mode
  stateDEcel  //decel state
} typedef State;

State currentState;
State requestedState;


/*
   UI ISR prototypes 
*/

void ISR_AA();  // ISR for active assist button
void ISR_SS();  // ISR for set speed button
void ISR_JS();  // ISR for joystick button
void ISR_TN();  // ISR for turn button
void ISR_UP();  // ISR for up button
void ISR_DN();  // ISR for down button

long debounceThresh = 1500;
// stores previous trigger time for buttons
long prevAA;
long prevJS;
long prevSS;
long prevUP;
long prevDN;
long prevTN;

float vel_sp;

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

//debounce function - limit button pressing using timer
bool debounceCheck(long& prevTime) {
  if (millis() - prevTime >= debounceThresh) {
    prevTime = millis();
    return true;
  }
  else {
    return false;
  }
}


void ISR_AA() {
  if (debounceCheck(prevAA)) {
    if (currentState == stateDE) {
      currentState = stateAA;
    } else if (currentState == stateAA) {
      // go to decel state first
      currentState = stateDEcel;
      // go to default after decel state
      requestedState = stateDE;
      // turn off LED
      digitalWrite(lAA, LOW);
    }
  }
}

void ISR_JS() {
  if (debounceCheck(prevJS)) {
    if (currentState == stateDE) {
      currentState = stateJS;
    } else if (currentState == stateJS) {
      // go to decel state first
      currentState = stateDEcel;
      // go to default after decel state
      requestedState = stateDE;
      // turn off LED
      digitalWrite(lJS, LOW);
    }
  }
}

void ISR_SS() {
  if (debounceCheck(prevSS)) {
    if (currentState == stateDE) {
      currentState = stateSS;
    } else if (currentState == stateSS) {
      // go to decel state first
      currentState = stateDEcel;
      // go to default after decel state
      requestedState = stateDE;
      // turn off LED
      digitalWrite(lAA, LOW);
    }
  }
}

void ISR_UP() {
  if (debounceCheck(prevUP)) {
    if (currentState == stateSS && vel_sp < maxSpeed) {
      vel_sp = vel_sp + dSpeed;
    }
  }
}

void ISR_DN() {
  if (debounceCheck(prevDN)) {
    if (currentState == stateSS && vel_sp > minSpeed) {
      vel_sp = vel_sp - dSpeed;
    }
  }
}

void ISR_TN() {
  if (debounceCheck(prevTN)) {
    if (currentState == stateSS) {
      currentState = stateTurnSS;
    }
    else if (currentState == stateAA) {
      currentState = stateTurnAA;
    }
    else if (currentState == stateTurnSS) {
      currentState = stateSS;
      digitalWrite(lTN, LOW);
    }
    else if (currentState == stateTurnAA) {
      currentState = stateAA;
      digitalWrite(lTN, LOW);
    }
    else {
      //if in neither do nothing haha
    }
  }
}
