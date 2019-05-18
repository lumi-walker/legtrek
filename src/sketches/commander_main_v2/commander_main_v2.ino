
#include "UI_utils.h"
#include "ui_pin_assignments.h"
#include "Motor.h"

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
  stateDecel  //decel state
} typedef State;

// flag that is true when stateDecel is complete
// move onto requested State
bool decelComplete;

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
void ISR_MOTOR_STOPPED(); //  ISR for when the motor stopped

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
  Serial.begin(9600);

  //initialize button
  pinMode(bAA,INPUT_PULLUP);
  pinMode(lAA,OUTPUT);
  pinMode(bJS,INPUT_PULLUP);
  pinMode(lJS,OUTPUT);
  pinMode(bSS,INPUT_PULLUP);
  pinMode(lSS,OUTPUT);
  pinMode(bUP,INPUT_PULLUP);
  pinMode(bDN,INPUT_PULLUP);
  pinMode(bTN,INPUT_PULLUP);
  pinMode(lTN,OUTPUT);
  
  //digital pin interrupts
  attachInterrupt(digitalPinToInterrupt(bAA),ISR_AA,FALLING);
  attachInterrupt(digitalPinToInterrupt(bJS),ISR_JS,FALLING);
  attachInterrupt(digitalPinToInterrupt(bSS),ISR_SS,FALLING);
  attachInterrupt(digitalPinToInterrupt(bUP),ISR_UP,FALLING);
  attachInterrupt(digitalPinToInterrupt(bDN),ISR_DN,FALLING);
  attachInterrupt(digitalPinToInterrupt(bTN),ISR_TN,FALLING);

  // pin which indicates if motor is running or not
  attachInterrupt(digitalPinToInterrupt(16),ISR_MOTOR_STOPPED,RISING);
  //attachInterrupt(digitalPinToInterrupt(M2_OUT3),ISR_MOTOR_STOPPED,RISING);
  
  interrupts();
  currentState = stateDE;

  // init debounce timers
  prevAA = millis();
  prevJS = millis();
  prevSS = millis();
  prevUP = millis();
  prevDN = millis();
  prevTN = millis();

}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(currentState);
  delay(500);
  switch(currentState) {
    case stateDE:
    //DEFAULT MODE
      
      break;
    case stateAA:
    //ACTIVE ASSIST MODE
     
      break;
    case stateJS:
    //JOYSTICK MODE
     
      break;
    case stateSS:
    //SETSPEED MODE
      
      break;
    case stateCE:
    //CRITICAL ERROR MODE
      
      break;
    case stateTurnAA:
    //TURNING IN ACTIVE ASSIST
    
      break;
    case stateTurnSS:
    //TURNING IN SET SPEED
  
      break;
      
    case stateSit:
    //SITTING MODE
   
      break;
      
    case stateDecel:
    //SITTING MODE
    
      if(decelComplete) {
        faststopon_all();
        currentState = requestedState;
        decelComplete = false;
      }
      break;
      
    default:
    // DEFAULT MODE
    
      break;
  }
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
      currentState = stateDecel;
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
      currentState = stateDecel;
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
      currentState = stateDecel;
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

void ISR_MOTOR_STOPPED() {
  decelComplete = true;
}
