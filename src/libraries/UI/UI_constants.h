#ifndef UI_CONSTANTS_H
#define UI_CONSTANTS_H


#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "ui_pin_assignments.h"
//constants


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
  stateDecel //deceling mode
} typedef State;



int batterylvl = 100;

//joystick state constants
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

//Error logic variables
//bool is_FAULTY_CURRENT_SENSOR = 0;
bool is_MOTOR_ERROR = 0;
bool is_FAULTY_TEMP_SENSOR = 0;
bool is_OVER_TEMP = 0;
//bool is_OVER_CURRENT = 0;
bool is_OVER_VOLT = 0;
bool is_LOW_VOLT = 0;

//universal LCD variables
//Run the LCD software from Adafruit
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
//LCD full dimensions
uint16_t  xmax = tft.width();
uint16_t  ymax = tft.height();
//battery life and box dimensions
uint16_t  dx = tft.width()/7;
uint16_t  dy = tft.height()/6;

//debounce for buttons
long debounceThresh = 500; //threshold in milliseconds

//booting stages
bool isBooting = 1;
bool isBootUp = 0;
bool isBootDown = 0;

#endif
