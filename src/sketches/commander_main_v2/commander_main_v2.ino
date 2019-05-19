
#include "UI_utils.h"
#include "ui_pin_assignments.h"
#include "Motor.h"

// state-machine typedef
enum {
  stateDE, // 0 Default mode
  stateAA, // 1 Active Assist mode
  stateJS, // 2 Joystick mode
  stateSS, // 3 Set Speed mode
  stateCE, // 4 Crtical Error mode
  stateTurnAA, // 5 Turning in AA
  stateTurnSS, // 6 Turning in SS
  stateSit, // 7 Sitting mode
  stateDecel  // 8decel state
} typedef State;

// flag that is true when stateDecel is complete
// move onto requested State
bool decelComplete = false;

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

float speed_sp;
double ang_sp;

void ISR_MOTOR_STOPPED() {
  decelComplete = true;
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

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

  //digital pin interrupts
  attachInterrupt(digitalPinToInterrupt(bAA), ISR_AA, FALLING);
  attachInterrupt(digitalPinToInterrupt(bJS), ISR_JS, FALLING);
  attachInterrupt(digitalPinToInterrupt(bSS), ISR_SS, FALLING);
  attachInterrupt(digitalPinToInterrupt(bUP), ISR_UP, FALLING);
  attachInterrupt(digitalPinToInterrupt(bDN), ISR_DN, FALLING);
  attachInterrupt(digitalPinToInterrupt(bTN), ISR_TN, FALLING);

  // REMOVE
  attachInterrupt(digitalPinToInterrupt(16), ISR_MOTOR_STOPPED, FALLING);
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

  switch (currentState) {
    case stateDE:
      //DEFAULT MODE
      // stop
      //all led LOW
      speed_sp = 0;
      ang_sp = 0;
      drive(speed_sp, ang_sp);
      break;
    case stateAA:
      //ACTIVE ASSIST MODE

      break;
    case stateJS:
      //JOYSTICK MODE

      break;
    case stateSS:
      //SETSPEED MODE
      drive(speed_sp, ang_sp);
      break;
    case stateCE:
      //CRITICAL ERROR MODE

      break;
    case stateTurnAA:
      //TURNING IN ACTIVE ASSIST

      break;
    case stateTurnSS:
      //TURNING IN SET SPEED
      readJoystick();
      //Serial.println("angle : " + String(angRead) + " || radius " + String(rRead));
      // determine speed
      if (rRead > rDeadBand) {
        speed_sp = (rRead - rDeadBand) * speed_sp + minSpeed;

        // valid radius -> determine direction
        if (angRead < rturn_max || angRead > rturn_min) {
          // right turn
          Serial.println("RIGHT TURN : " + String(speed_sp));
          ang_sp = 0; // radians
        } else if (angRead < lturn_max && angRead > lturn_min) {
          // left turn
          Serial.println("LEFT TURN : " + String(speed_sp));
          ang_sp = PI;
        } else {
          // not valid direction for turnSS -> do not move
          Serial.println("NO TURN : ");
          ang_sp = 0;
          speed_sp = 0;
        }
      } else if (angRead >= back_min && angRead <= back_max && rRead > rDeadBand * 5) {
        speed_sp = .3;
        ang_sp = 3 * PI / 2;
        Serial.println("BACK!!!");
      } else {

        // not valid radius -> do not move
        speed_sp = 0;
        Serial.println("Deadband");
      }
  }

  drive(speed_sp, ang_sp);

  break;

case stateSit:
  //SITTING MODE

  break;

case stateDecel:
  // DECEL MODE
  speed_sp = 0;
  drive(speed_sp, ang_sp);

  if (isMotorRunning() == false) {
    currentState = requestedState;
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
    if (currentState == stateSS && speed_sp > minSpeed) {
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
      digitalWrite(lTN, LOW);
    }
    else if (currentState == stateTurnAA) {
      requestedState = stateAA;
      currentState = stateDecel;
      digitalWrite(lTN, LOW);
    }
  }
}
