
#include "UI_utils.h"
#include "ui_pin_assignments.h"
#include "Motor.h"
int CrossPin1 = 2;  // input pin for break beam sensor
int CrossPin2 = 8;

String readString;



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


int isObstacle = HIGH;  // HIGH MEANS NO OBSTACLE

int initflag = 0;
bool resetSpeed = false;

long steptime = 600000; //1000000micro sec = 1s .
long dsteptime = 50000;



long starttime = 0;

bool decellTime = false;
long startTime_decell = -1;


/*
   UI ISR prototypes
*/

void ISR_AA();  // ISR for active assist button
void ISR_SS();  // ISR for set speed button
void ISR_JS();  // ISR for joystick button
void ISR_TN();  // ISR for turn button
void ISR_UP();  // ISR for up button
void ISR_DN();  // ISR for down button
long debounceThresh = 500;

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

  motor_init();
  motor_ready();
}

void loop() {
  Serial.println(currentState);
  checkinput();
  switch (currentState) {
    case stateDE:
      //DEFAULT MODE
      // stop
      //all led LOW
      speed_sp = 0;
      ang_sp = 0;
      drive(speed_sp, ang_sp);
      digitalWrite(lAA, LOW);
      digitalWrite(lJS, LOW);
      digitalWrite(lSS, LOW);
      digitalWrite(lTN, LOW);
      break;
    case stateAA:
      //ACTIVE ASSIST MODE
      Serial.println("IR:"+String(digitalRead(CrossPin1))+","+digitalRead(CrossPin2));

      speed_sp = .45;



      if (initflag) { //if interrupt happens
        drive(speed_sp, PI / 2);
        initflag = 0;
        Serial.println("start moving");
        resetSpeed = true;
      }
      if (micros() - starttime > steptime && resetSpeed == true) {
        drive(0, PI / 2);
        resetSpeed = false;
      }
      buttonBlink(lAA);
      break;
    case stateJS:
      //JOYSTICK MODE
      readJoystick();
      Serial.println("angle : " + String(angRead) + " || radius " + String(rRead));
      // determine speed
      if (rRead > rDeadBand) {
        speed_sp = (rRead - rDeadBand) * speed_sp + minSpeed;

        // valid radius -> determine direction
        if (angRead < rturn_max  || angRead > rturn_min) {
          // right turn
          Serial.println("RIGHT TURN : " + String(speed_sp));
          ang_sp = 0; // radians
        } else if (angRead < lturn_max && angRead > lturn_min) {
          // left turn
          Serial.println("LEFT TURN : " + String(speed_sp));
          ang_sp = PI;
        } else if (angRead >= forward_min && angRead <= forward_max && currentState == stateJS) {
          ang_sp = PI / 2;
          Serial.println("FORWARD");
        } else if (angRead >= back_min && angRead <= back_max && rRead > rDeadBand * 5) {
          speed_sp = .3;
          ang_sp = 3 * PI / 2;
          Serial.println("BACK!!!");
        } else {
          // not valid direction for turnSS -> do not move
          Serial.println("NO MOVING");
          ang_sp = 0;
          speed_sp = 0;
        }
      }  else {

        // not valid radius -> do not move
        speed_sp = 0;
        Serial.println("Deadband");
      }


      drive(speed_sp, ang_sp);

      buttonBlink(lJS);

      break;
    case stateSS:
      //SETSPEED MODE
      drive(speed_sp, PI/2);
      buttonBlink(lSS);
      break;
    case stateCE:
      //CRITICAL ERROR MODE
      drive(0, PI / 2);
      break;
    case stateTurnAA:
      //TURNING IN ACTIVE ASSIST
      buttonBlink(lTN);
      buttonBlink(lAA);
      readJoystick();
      Serial.println("angle : " + String(angRead) + " || radius " + String(rRead));
      // determine speed
      if (rRead > rDeadBand) {
        speed_sp = (rRead - rDeadBand) * speed_sp + minSpeed;

        // valid radius -> determine direction
        if (angRead < rturn_max  || angRead > rturn_min) {
          // right turn
          Serial.println("RIGHT TURN : " + String(speed_sp));
          ang_sp = 0; // radians
        } else if (angRead < lturn_max && angRead > lturn_min) {
          // left turn
          Serial.println("LEFT TURN : " + String(speed_sp));
          ang_sp = PI;
        } else if (angRead >= back_min && angRead <= back_max && rRead > rDeadBand * 5) {
          speed_sp = .3;
          ang_sp = 3 * PI / 2;
          Serial.println("BACK!!!");
        } else {
          // not valid direction for turnSS -> do not move
          Serial.println("NO TURN : ");
          ang_sp = 0;
          speed_sp = 0;
        }
      }  else {

        // not valid radius -> do not move
        speed_sp = 0;
        Serial.println("Deadband");
      }


      drive(speed_sp, ang_sp);

      break;
    case stateTurnSS:
      //TURNING IN SET SPEED
      buttonBlink(lTN);
      buttonBlink(lSS);
      readJoystick();
      Serial.println("angle : " + String(angRead) + " || radius " + String(rRead));
      // determine speed
      if (rRead > rDeadBand) {
        speed_sp = (rRead - rDeadBand) * speed_sp + minSpeed;

        // valid radius -> determine direction
        if (angRead < rturn_max  || angRead > rturn_min) {
          // right turn
          Serial.println("RIGHT TURN : " + String(speed_sp));
          ang_sp = 0; // radians
        } else if (angRead < lturn_max && angRead > lturn_min) {
          // left turn
          Serial.println("LEFT TURN : " + String(speed_sp));
          ang_sp = PI;
        } else if (angRead >= back_min && angRead <= back_max && rRead > rDeadBand * 5) {
          speed_sp = .3;
          ang_sp = 3 * PI / 2;
          Serial.println("BACK!!!");
        } else {
          // not valid direction for turnSS -> do not move
          Serial.println("NO TURN : ");
          ang_sp = 0;
          speed_sp = 0;
        }
      }  else {

        // not valid radius -> do not move
        speed_sp = 0;
        Serial.println("Deadband");
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
      Serial.println("motor status" + String(isMotorRunning()));
      if (isMotorRunning() == false) {
        currentState = requestedState;
      }

      break;
  }
  //
  //default:
  //  // DEFAULT MODE
  //  break;
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
    }
    else if (currentState == stateAA) {
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
      digitalWrite(lSS, LOW);
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
//    else if (currentState == stateAA && speed_sp < 1.0f) {
//      if (speed_sp == 0) {
//        speed_sp = minSpeed;
//      } else {
//
//        speed_sp = speed_sp + dSpeed;
//        if (speed_sp > 1.0f) {
//          speed_sp = 1.0f;
//        }
//      }
//    }else if (currentState == stateTurnAA ) {
//      if (speed_sp == 0) {
//        speed_sp = minSpeed;
//      } else {
//
//        steptime = steptime + dsteptime;
//        if (steptime > 12000000) {
//          steptime > 12000000;
//        }
//      }
//    }
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
      digitalWrite(lTN, LOW);
    }
    else if (currentState == stateTurnAA) {
      requestedState = stateAA;
      currentState = stateDecel;
      digitalWrite(lTN, LOW);
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


void checkinput() {
  // take value from serial monitor and determine states accordingly
  // !!!all input from serial monitor must start with a number and end with a comma!!!
  if (Serial.available() > 0) { //if there's a input from serial monitor
    char c = Serial.read();  //gets one byte from serial buffer

    if (c == ',') {
      if (readString.length() > 1) {
        Serial.println(readString); //prints string to serial port out

        int n = readString.toInt();  //convert readString into a number

        if (readString.indexOf("t") > 0) {

          steptime = n*1000;
          Serial.println("steptime = " + String(steptime/1000) + "ms");
        }
        else if (readString.indexOf("v") > 0) {
          speed_sp = float(n) / 10.0f;
          Serial.println("vel = " + String(speed_sp));
        }
        
        else {
          Serial.println("ERROR! Follow input Format!!! Previous input cleared!");
          readString = ""; //clears variable for new input
        }
        readString = ""; //clears variable for new input
      }
    }
    else {
      readString += c; //makes the string readString
    }

  }

}

