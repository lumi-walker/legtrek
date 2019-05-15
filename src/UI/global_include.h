//global variable definitions

//include necessary files
#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include "math.h"
//#include "BMSUtils.h"
//#include "motor.h"
#include "ui_pin_assignments.h"

//Run the LCD software from Adafruit
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//define PI
#define PI 3.1415926535897932384626433832795

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

State currState = stateDE; //current state
State prevState = stateDE;


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

bool stateTN = 0; //Turn mode
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

//set speed parameters
float setSpeed = 0.0; //Speed set to motors
float prevSpeed = setSpeed; //prevSpeed
float dSpeed = 0.1; //speed change
float minSpeed = 0.0; //minimum limit in set speed
float maxSpeed = 1.5; //maximum limit in set speed

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

enum {
	FAULTY_TEMPERATURE_SENSOR,
	OVER_TEMPERATURE, // below here is critical error
	OVER_VOLTAGE,
	LOW_VOLTAGE,
	NUM_MESSAGES
} typedef BMSErrorMessage;

enum {
  SITTING_MODE,
  STANDING_MODE
} typedef Prox;
