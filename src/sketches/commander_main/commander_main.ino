//State Machine Logic
#include "ui_functions_init.h"
#include "Motor.h"

// comment this out to not print
#define PRINT_DEBUG


// UI function prototypes, definitions below
void runDefaultMode();
void runActiveAssistMode();
void runJoystickMode();
void runSetSpeedMode();
void runTurningMode();
void runCriticalErrorMode();
void runSittingMode();
void ShutDownProcedure(ShutDown stage);
void printStatus();

/*
 * ACTIVE ASSIST REGION
 * relocate this bit of code later if logically necessary
 */
 
// active assist util variables
long debounceThreshold = 200000;

// period of time to move forward when IR sensors triggered
long strideStepTime = 3000000;
// vel setpoint when IR sensors triggered in mph
float AA_vel_sp = 0.8;

// last time the IR sensor was triggered
volatile long prevTrigTime;

// start time of valid IR sensor trigger
volatile bool initStride;
long strideStartTime;

// active assist IR pin
int IRpin = 53;

// active assist ISR 
void ISR_AA_sensor() {
  
  if(currState == stateAA) {
     if(micros() - prevTrigTime > debounceThreshold) {
        prevTrigTime = micros();
        strideStartTime = micros();
       
        if(vel_sp == 0) {
          initStride = true;
        }
     }
  }
}

void setup() {
  // put your setup code here, to run once:
  UI_setup();
  motor_init();
  motor_ready();
  pinMode(IRpin,INPUT);
  attachInterrupt(digitalPinToInterrupt(IRpin),ISR_AA_sensor,FALLING);
}

void loop() {
  if (currState != prevState) { //only change LCD if the state is changed
    LCD.writeMode(currState);
    prevState = currState;
  }
  // implement a timer next
  if(zero_out_velocity) {
    decel_to_zero();
    zero_out_velocity = false;
  }
  
  switch(currState) {
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
      runTurningMode();
      break;
    case stateTurnSS:
    //TURNING IN SET SPEED
      runTurningMode();
      break;
      
    case stateSit:
    //SITTING MODE
      runSittingMode();
      break;
      
    default:
      runDefaultMode();
      break;
  }


  #ifdef PRINT_DEBUG
    printStatus();
  #endif
}


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
      decel_to_zero();
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
          decel_to_zero();
          stateRTN = true;
          stateLTN = false;
          vel_sp = turn_vel_sp;
          turnRight(vel_sp);
        }  
    } else if(angRead < validAngleRangeLTurn_max && angRead > validAngleRangeLTurn_min) {
      // left turn

        if(stateLTN == false) {
          decel_to_zero();
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
    decel_to_zero();
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
            decel_to_zero();
            stateRTN = true;
            stateLTN = false;
            vel_sp = turn_vel_sp;
            turnRight(vel_sp);
          }  
    } else if(angRead < validAngleRangeLTurn_max && angRead > validAngleRangeLTurn_min) {
      // turn left
      Serial.println("TURN LEFT");
          if(stateLTN == false) {
            decel_to_zero();
            stateRTN = false;
            stateLTN = true;
            vel_sp = turn_vel_sp;
            turnLeft(vel_sp);
          }  
    } else {
          stateRTN = false;
          stateLTN = false;
          decel_to_zero();  
    }
  } else {
          stateRTN = false;
          stateLTN = false;
          decel_to_zero();  
  }
}

void runCriticalErrorMode(){
  vel_sp = 0.0;
}

void runSittingMode() {

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

void printStatus() {
  String currentState;
  String vel;
  String stats;
  
  if(currState == stateDE) {
    currentState = "DEFAULT";
  }
  
  if(currState == stateAA) {
    currentState = "ACTIVE ASSIST";
    vel = String(vel_sp);
    String motionState = vel_sp == 0 ? "NOT MOVING" : "MOVING";
    stats = "Velocity : " + vel + " || " + motionState; 
  }

  if(currState == stateJS) {
    currentState = "JOYSTICK";
    vel = String(vel_sp);
    stats = "Velocity : " + vel + " || rRead : " + String(rRead) + " || " + "angRead : " + String(angRead);
  }

  if(currState == stateSS) {
    currentState = "SET SPEED";
    vel = String(vel_sp);
    stats = "Velocity : " + vel;
  }

  if(currState == stateTurnAA) {
    currentState = "TURN - ACTIVE ASSIST";
    vel = String(vel_sp);
    
  }

  if(currState == stateTurnSS) {
    currentState = "TURN - SET SPEED";
  }

  if(currState == stateSit) {
    currentState = "SIT";
  }

  Serial.println("CURRENT STATE : " + currentState + " || " + stats);
}
