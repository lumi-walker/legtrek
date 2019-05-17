//global variable definitions

#ifndef GLOBAL_INCLUDES_H
#define GLOBAL_INCLUDES_H

//include necessary files
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "math.h"
#include "BMSUtils.h"
#include "Motor.h"
#include "ui_pin_assignments.h"

//Run the LCD software from Adafruit
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


//universal LCD variables
//LCD full dimensions
uint16_t  xmax = tft.width();
uint16_t  ymax = tft.height();
//battery life and box dimensions
uint16_t  dx = tft.width()/7;
uint16_t  dy = tft.height()/6;

//state variables
enum {
  stateDE, //Default mode
  stateAA, //Active Assist mode
  stateJS, //Joystick mode
  stateSS, //Set Speed mode
  stateCE, //Crtical Error mode
  stateTurnAA, //Turning in AA
  stateTurnSS, //Turning in SS
  stateSit //Sitting mode
} typedef State;

volatile State currState = stateDE; //current state
volatile State prevState = stateDE;


//Serial bytes
volatile byte incomingByte;

#define PING_TYPE_MASK (1<<7)

//Error logic variables
//bool is_FAULTY_CURRENT_SENSOR = 0;
bool is_MOTOR_ERROR = 0;
bool is_FAULTY_TEMP_SENSOR = 0;
bool is_OVER_TEMP = 0;
//bool is_OVER_CURRENT = 0;
bool is_OVER_VOLT = 0;
bool is_LOW_VOLT = 0;

#define CRITICAL_ERROR_MASK (1 << OVER_TEMPERATURE) || (1 << OVER_VOLTAGE) || (1 << LOW_VOLTAGE)

volatile bool zero_out_velocity = false;

bool stateRTN = 0; //Turn mode
bool stateLTN = 0;
//Joystick (js) modes
enum {
 jsBrake, //device is braked in turning mode
 jsForward, //device is moving forward in JS mode
 jsRight, //device is turning right in turn mode
 jsLeft, // device is turning left in turn mode
 jsBackward //device is going backwards
} typedef js;

js prevTurn = jsBrake; //intialize prevTurn to 0
js currTurn = jsBrake; //current turn mode

//timer variables
long debounceThresh = 1500; //threshold in milliseconds
//timers for each mode
long prevAA;
long prevJS;
long prevSS;
long prevUP;
long prevDN;
long prevTN;

bool was_depressed = 0; //might not use this
int hog = 0;

//joystick outputs
double angRead = 0; //angle of pitch
double rRead =  0; //magnitude of pitch
double rDeadBand = 0.1;
double forwardBearing = PI/2;
double validAngleRange_deg = 10;
double validAngleRange_rad = validAngleRange_deg * PI / 180.0f;

// joystick angular bounds for moving forward
double validAngleRangeForward_min = forwardBearing - validAngleRange_rad;
double validAngleRangeForward_max = forwardBearing + validAngleRange_rad;

// joystick angular bounds for right turn
double validAngleRangeRTurn_min = 0 - validAngleRange_rad;
double validAngleRangeRTurn_max = 0 + validAngleRange_rad;

// joystick angular bounds for left turn
double validAngleRangeLTurn_min = PI - validAngleRange_rad;
double validAngleRangeLTurn_max = PI + validAngleRange_rad;

//set speed parameters
float vel_sp = 0.0; //Speed set to motors
float turn_vel_sp = 0.7;
float prev_vel_sp = vel_sp; //prev_vel_sp
float dSpeed = 0.1; //speed change



//battery level (NEEDS CHANGE)
int batterylvl = 100; //actually should read from battery voltage
bool low_batt = 0; //low battery flash variable

//booting stages
bool isBooting = 1;
bool isBootUp = 0;
bool isBootDown = 0;

//shutdown stages
enum {
  TEENSY_TO_DUE,
  DUE_TURN_OFF_MOTOR,
  DUE_TO_TEENSY,
  TEENSY_TO_RELAYS,
  TEENSY_TO_DUE2,
  DUE_PRINT_LCD
} typedef ShutDown;

ShutDown currShutDown;

#endif
