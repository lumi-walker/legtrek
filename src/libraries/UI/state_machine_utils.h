#ifndef STATE_MACHINE_UTILS_H
#define STATE_MACHINE_UTILS_H
#include "Motor.h"
#include "UI_constants.h"
#include "UI_utils.h"
#include "LCD.h"

State currentState;
State prevState;
State requestedState;

// stores previous trigger time for buttons
long prevAA;
long prevJS;
long prevSS;
long prevUP;
long prevDN;
long prevTN;
long prevSit;

float speed_sp;
float prev_speed;
double ang_sp;

bool sitting = 0;
bool isConnected = 1;

//AA def
int initflag = 0;
bool resetSpeed = false;
long starttime = 0;
long steptime = 800000; //1000000micro sec = 1s .

void ISR_AA();  // ISR for active assist button
void ISR_SS();  // ISR for set speed button
void ISR_JS();  // ISR for joystick button
void ISR_TN();  // ISR for turn button
void ISR_UP();  // ISR for up button
void ISR_DN();  // ISR for down button
void ISR_AAstep(); //// ISR for IR sensor
void ISR_isSitting(); // ISR for proximity sensor
//------------initialize ------------------------------------------------------
void init_interrupts(){
  //initialize button
pinMode(bAA, INPUT_PULLUP);
pinMode(lAA, OUTPUT);
pinMode(bJS, INPUT_PULLUP);
pinMode(lJS, OUTPUT);
pinMode(bSS, INPUT_PULLUP);
pinMode(lSS, OUTPUT);
pinMode(bUP, INPUT_PULLUP);
pinMode(bDN, INPUT_PULLUP);
pinMode(bTN, INPUT_PULLUP);
pinMode(lTN, OUTPUT);
pinMode(CrossPin1, INPUT);
pinMode(CrossPin2, INPUT);

//digital pin interrupts
attachInterrupt(digitalPinToInterrupt(bAA), ISR_AA, FALLING);
attachInterrupt(digitalPinToInterrupt(bJS), ISR_JS, FALLING);
attachInterrupt(digitalPinToInterrupt(bSS), ISR_SS, FALLING);
attachInterrupt(digitalPinToInterrupt(bUP), ISR_UP, FALLING);
attachInterrupt(digitalPinToInterrupt(bDN), ISR_DN, FALLING);
attachInterrupt(digitalPinToInterrupt(bTN), ISR_TN, FALLING);
attachInterrupt(digitalPinToInterrupt(CrossPin1), ISR_AAstep, FALLING);
attachInterrupt(digitalPinToInterrupt(CrossPin2), ISR_AAstep, FALLING);
attachInterrupt(digitalPinToInterrupt(PROX_PIN), ISR_isSitting, CHANGE);
interrupts();
}
void init_timer(){
  // init debounce timers
  prevAA = millis();
  prevJS = millis();
  prevSS = millis();
  prevUP = millis();
  prevDN = millis();
  prevTN = millis();
  prevSit = millis();
}
//------------state machine functions------------------------------------------
void runDefaultMode() {
// stop
//all led LOW
speed_sp = 0;
ang_sp = 0;
drive(speed_sp, ang_sp);
// faststopon_all();
digitalWrite(lAA, LOW);
digitalWrite(lJS, LOW);
digitalWrite(lSS, LOW);
digitalWrite(lTN, LOW);
}

void runActiveAssistMode() {
  digitalWrite(lTN, LOW);
        Serial.println("IR:" + String(digitalRead(CrossPin1)) + "," + digitalRead(CrossPin2));
        speed_sp = .45;
        if (initflag) { //if interrupt happens
          drive(speed_sp, PI / 2);
          initflag = 0;
          Serial.println("start moving");
          resetSpeed = true;
          currTurn = jsForward;
        }
        if (micros() - starttime > steptime && resetSpeed == true) {
          resetSpeed = false;
          speed_sp = 0;
          drive(speed_sp, PI / 2);
          currTurn = jsBrake;
        }
        buttonBlink(lAA);
}

void runJoystickMode() {
  readJoystick();
  Serial.println("angle : " + String(angRead) + " || radius " + String(rRead));
  // determine speed
  if (rRead > rDeadBand) {
    //------------why----------------------------------------
    // speed_sp = (rRead - rDeadBand) * speed_sp + minSpeed;
    // speed_sp = (rRead - rDeadBand)* (maxSpeed-minSpeed) +minSpeed;
    speed_sp = float(0.3056*rRead*rRead + 0.5083*rRead + 0.1861); //parabola for smoother transition
    if (speed_sp>1) {
      speed_sp = 1;
    }
    // valid radius -> determine direction
    if (angRead < rturn_max  || angRead > rturn_min) {
      // right turn
      Serial.println("RIGHT TURN : " + String(speed_sp));
      currTurn = jsRight;
      ang_sp = 0; // radians
      speed_sp = speed_sp/2; //slower speed when turning
    } else if (angRead < lturn_max && angRead > lturn_min) {
      // left turn
      Serial.println("LEFT TURN : " + String(speed_sp));
      currTurn = jsLeft;
      ang_sp = PI;
      speed_sp = speed_sp/2; //slower speed when turning
    } else if (angRead >= forward_min && angRead <= forward_max) {
      ang_sp = PI / 2;
      currTurn = jsForward;
      Serial.println("FORWARD");
    } else if (angRead >= back_min && angRead <= back_max && rRead > rDeadBand * 7) {
      speed_sp = .3;
      ang_sp = 3 * PI / 2;
      currTurn = jsBackward;
      Serial.println("BACK!!!");
    } else {
      // not valid direction for turn -> do not move
      if (currentState == stateSit && ((angRead >= rturn_max && angRead <= forward_min) || (angRead >= forward_max && angRead <= lturn_min))){
        //enable turning in arc
          ang_sp = angRead;
          Serial.println("ARC");
      }else {
        Serial.println("NO MOVING");
        ang_sp = 0;
        speed_sp = 0;
        currTurn = jsBrake;
      }
    }
  }  else {
    // not valid radius -> do not move
    speed_sp = 0;
    currTurn = jsBrake;
    Serial.println("Deadband");
  }
  drive(speed_sp, ang_sp);
  if (currentState == stateJS || currentState == stateSit ){
    buttonBlink(lJS);
    buttonBlink(lTN);
  } else if (currentState == stateTurnAA){
    buttonBlink(lTN);
    buttonBlink(lAA);
  } else if (currentState == stateTurnSS){
    buttonBlink(lTN);
    buttonBlink(lSS);
  }

}

void runSetSpeedMode() {
  digitalWrite(lTN, LOW);
  if (speed_sp != prev_speed) { //write speed if speed_sp has updated;
    LCD.writeSpeed(speed_sp);
    prev_speed = speed_sp;
  }
  if (speed_sp > 0) {
    currTurn = jsForward;
  }else {
    currTurn = jsBrake;
  }
  drive(speed_sp, PI / 2);
  buttonBlink(lSS);
}


void runCriticalErrorMode(){
  speed_sp = 0.0;
  drive(speed_sp, PI / 2);
  if (isConnected = 1){
    relayController.disconnect(); //disconnect power
  }
  LCD.batteryRemoval(); //indicate user that it's safe to remove battery
}

void runDecelMode(){
  speed_sp = 0;
drive(speed_sp, ang_sp);
Serial.println("motor status" + String(isMotorRunning()));

// if (isMotorRunning() == false) {
  currentState = requestedState;
// }
}

//-----------------------------------------------------------------------------
//debounce function - limit button pressing using timer
bool debounceCheck(long& prevTime) {
  if (micros() - prevTime >= debounceThresh) {
    prevTime = micros();
    return true;
  }else {
    return false;
  }
}


void ISR_AA() {
  if (debounceCheck(prevAA)) {
    if (currentState == stateDE) {
      currentState = stateAA;
    }else if (currentState == stateAA) {
      // go to decel state first
      currentState = stateDecel;
      // go to default after decel state
      requestedState = stateDE;
    }
  }

}

void ISR_JS() {
  if (debounceCheck(prevJS)) {
    if (currentState == stateDE) {
      if (sitting == 1){
        currentState = stateSit;
      } else{
        currentState = stateJS;
      }
    } else if (currentState == stateSit || currentState == stateJS) {
      // go to decel state first
      currentState = stateDecel;
      // go to default after decel state
      requestedState = stateDE;
    }
  }
}

void ISR_SS() {
  if (debounceCheck(prevSS)) {
    if (currentState == stateDE) {
      currentState = stateSS;
    } else if (currentState == stateSS) {
      // go to decel state first
      currentState = stateDecel;
      // go to default after decel state
      requestedState = stateDE;
    }
  }
}

void ISR_UP() {
  if (debounceCheck(prevUP)) {
    if (currentState == stateSS && speed_sp < maxSpeed) {
      if (speed_sp == 0) {
        speed_sp = minSpeed;
      } else {

        speed_sp = speed_sp + dSpeed;
        if (speed_sp > maxSpeed) {
          speed_sp = maxSpeed;
        }
      }
    }
  }
}

void ISR_DN() {
  if (debounceCheck(prevDN)) {
    if (currentState == stateSS && speed_sp >= minSpeed) {
      speed_sp -= dSpeed;
      if (speed_sp < minSpeed) speed_sp = 0;
    }
  }
}

void ISR_TN() {
  if (debounceCheck(prevTN)) {
    if (currentState == stateSS) {
      requestedState = stateTurnSS;
      currentState = stateDecel;
    }
    else if (currentState == stateAA) {
      requestedState = stateTurnAA;
      currentState = stateDecel;
    }
    else if (currentState == stateTurnSS) {
      requestedState = stateSS;
      currentState = stateDecel;
    }
    else if (currentState == stateTurnAA) {
      requestedState = stateAA;
      currentState = stateDecel;
    }else if (currentState == stateDE && sitting == 0){
      requestedState = stateJS;
      currentState = stateDecel;
    }else if (currentState == stateJS){
      requestedState = stateDE;
      currentState = stateDecel;
    }else if (currentState == stateDE && sitting == 1){
      requestedState = stateSit;
      currentState = stateDecel;
    }else if (currentState == stateSit){
      requestedState = stateDE;
      currentState = stateDecel;
    }
  }
}

void ISR_AAstep() {
  if (micros() - starttime > debounceThresh*1000) {
//    //    if (micros() - starttime > steptime) {
    initflag = 1;
//    //    }
    starttime = micros();//time when a step initiated
  }
}

void ISR_isSitting(){
  if (debounceCheck(prevSit)) {
    sitting = 0;
    if (digitalRead(PROX_PIN) == 1){
      bool sitting = 1;
    }
  }
}

#endif
