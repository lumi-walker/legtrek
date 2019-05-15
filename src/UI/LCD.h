//LCD Screen Functions and Classes

#include "global_include.h"

/*GENERAL STRUCTURE OF WRITING ONTO LCD

 FOR TEXT:
  tft.setCursor(x_coordinate,y_coordinate)
  *coordinates based on top left corner as origin

  tft.setTextColor(ILI9341_COLOR)
  *COLOR options:BLACK,BLUE,RED,GREEN,CYAN,MAGENTA,MAGENTA,YELLOW,WHITE
  *replace COLOR with one of these colors to set color

  tft.setTextSize(font_size);
  *fontsize typically between 3-6

  tft.println("DESIRED_TEXT")
   *display the text DESIRED_TEXT

 FOR DRAWING SHAPES:

  tft.fillRect(x0,y0,dx,dy,ILI9341_COLOR)
  *x0,y0 is the top left corner of where the rectangle will be
  *dx and dy are the extensions from that corner to create rectangle

  *other functions include: fillRect,fillCircle,drawRect,drawCircle,fillScreen
  *drawPixel,drawLine,drawRoundRect, fillRoundRect,drawTriangle,fillTriangle
  *drawChar

  *All have different parameters

  More info at: https://learn.adafruit.com/adafruit-gfx-graphics-library/coordinate-system-and-units
 */
class LCD {
  public:
    LCD() {

    }

    ~LCD() {

    }

    //Functions for the LCD

    //loading screen for during the boot-up phase
    void loadingScreen();
    //start-up screen for permanent text on the lCD
    void startScreen();
    //update battery life bar based on battery voltage
    void writeBatteryLevel(int) ;
    //update error panel based on errors detected from TEENSY
    void writeErrorPanel();
    //write speed onto LCD
    void writeSpeed(float);
    //update the mode that machine is in on lCD
    void writeMode(State);
    //write the state of the turn
    void writeTurn(js);
    //write the different modes
    void writeDE(); //Default
    void writeAA(); //Active Assist
    void writeJS(); //Joystick
    void writeSS(); //Set Speed
    void writeCE(); //Critical Error
    void writeSitMode();
    //In critical error: display when it is ok to remove battery
    void batteryRemoval();
};

//play the loading screen until the TEENSY responds to DUE (boot up complete)
void LCD::loadingScreen() {
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(10,dy*2.5);
  tft.setTextColor(ILI9341_BLACK);  tft.setTextSize(5);
  tft.println("LEGtrek");
  tft.setCursor(10,dy*3.3);
  tft.setTextColor(ILI9341_BLUE);  tft.setTextSize(2);
  tft.println("   Walk empowered");
//until bootup complete and after one second
  while(!isBootUp) {
    tft.fillCircle(dx,dy*4.5,10,ILI9341_BLACK);
    yield();
    delay(200);
    tft.fillCircle(2.5*dx,dy*4.5,10,ILI9341_BLACK);
    yield();
    delay(200);
    tft.fillCircle(4*dx,dy*4.5,10,ILI9341_BLACK);
    yield();
    delay(200);
    tft.fillCircle(5.5*dx,dy*4.5,10,ILI9341_BLACK);
    yield();
    delay(200);
    tft.fillCircle(1*dx,dy*4.5,10,ILI9341_WHITE);
    tft.fillCircle(2.5*dx,dy*4.5,10,ILI9341_WHITE);
    tft.fillCircle(4*dx,dy*4.5,10,ILI9341_WHITE);
    tft.fillCircle(5.5*dx,dy*4.5,10,ILI9341_WHITE);
    delay(200);
    //in real code, take out (or add) if condition, set digital read to ping pin
    if (millis()>1000) {
      isBootUp = !digitalRead(24);
    }
  }
}


//After boot-up, make screen where some things are permanent
void LCD::startScreen() {
  tft.fillRect(0,0,xmax,3*dy,ILI9341_CYAN);
  tft.fillRect(0,3*dy,xmax,3*dy,ILI9341_BLUE);

  uint16_t xSpeed = 60;
  uint16_t ySpeed = 70;
  tft.setCursor(xSpeed,ySpeed);
  tft.setTextColor(ILI9341_BLACK); tft.setTextSize(3);
  tft.println("SPEED");

  uint16_t xMPH = 4*dx-10;
  uint16_t yMPH = 2*dy+5;
  tft.setCursor(xMPH,yMPH);
  tft.setTextColor(ILI9341_BLACK); tft.setTextSize(3);
  tft.println("mph");

  tft.setCursor(70,3.2*dy+10);
  tft.setTextColor(ILI9341_WHITE); tft.setTextSize(3);
  tft.println("MODE");

  //write variable text using global variables
  writeBatteryLevel(batterylvl); //read from voltage
  writeErrorPanel();
  writeSpeed(setSpeed); //should be 0
  writeMode(stateDE); //should be default
  writeTurn(currTurn); //should be braked
}

//battery life of the device displayed on the top row in terms of percentage blocks
void LCD::writeBatteryLevel(int voltage) {
  //width of each percentage box
  uint16_t  bdx = tft.width()*6/70;
  //any percentage box is now the background color
  tft.fillRect(0,0,6*dx-5,dy,ILI9341_CYAN);
  //for each percentage block, display based on the voltage reading from teensy
  if (voltage >= 90) {
    tft.fillRect(0,0,bdx,dy, ILI9341_GREEN);
    tft.drawRect(0,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 80) {
    tft.fillRect(bdx,0,bdx,dy, ILI9341_GREEN);
    tft.drawRect(bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 70) {
    tft.fillRect(2*bdx,0,bdx,dy, ILI9341_GREEN);
    tft.drawRect(2*bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 60) {
    tft.fillRect(3*bdx,0,bdx,dy, ILI9341_ORANGE);
    tft.drawRect(3*bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 50) {
    tft.fillRect(4*bdx,0,bdx,dy, ILI9341_ORANGE);
    tft.drawRect(4*bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 40) {
    tft.fillRect(5*bdx,0,bdx,dy, ILI9341_ORANGE);
    tft.drawRect(5*bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 30) {
    tft.fillRect(6*bdx,0,bdx,dy, ILI9341_RED);
    tft.drawRect(6*bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 20) {
    tft.fillRect(7*bdx,0,bdx,dy, ILI9341_RED);
    tft.drawRect(7*bdx,0,bdx,dy, ILI9341_BLACK);}
  if (voltage >= 10) {
    tft.fillRect(8*bdx,0,bdx,dy, ILI9341_RED);
    tft.drawRect(8*bdx,0,bdx,dy, ILI9341_BLACK);}
  //if within last 10%, flash the last box to indicate low levels
  if (voltage >= 0) {
      if ((millis() % 2000 <= 1000) && low_batt) {
        tft.fillRect(9*bdx,0,bdx,dy, ILI9341_CYAN);
        low_batt == 0;
      }
      else {
        tft.fillRect(9*bdx,0,bdx,dy, ILI9341_BLACK);
        tft.drawRect(9*bdx,0,bdx,dy, ILI9341_BLACK);
        low_batt == 1;
      }
    }
  //outline the battery box
  tft.drawRect(0,0,10*bdx,dy,ILI9341_BLACK);
}

//this uses global variables to update, only proc by ping
//signal green or red to errors if they occur onto the LCD (placed on the right column)
void LCD::writeErrorPanel() {
  //if detect motor error change box to red, otherwise remain green
  uint16_t x0 = 6;
  uint16_t xoff = 5;
  if(is_MOTOR_ERROR) { //CHECK WITH MIN LATER
    tft.fillRect(x0*dx-xoff,0,dx+2*xoff,dy, ILI9341_RED);
  }
  else {
  tft.fillRect(x0*dx-xoff,0,dx+2*xoff,dy, ILI9341_GREEN);
  }
  //if TEENSY detects bad temp sensor change box to red, otherwise remain green
  if(is_FAULTY_TEMP_SENSOR) {
    tft.fillRect(x0*dx-xoff,0,dx+2*xoff,dy, ILI9341_RED);
  }
  else {
    tft.fillRect(x0*dx-xoff,dy,dx+2*xoff,dy, ILI9341_GREEN);
  }
  //if detect over temperature, same action as above
  if(is_OVER_TEMP) {
    tft.fillRect(x0*dx-xoff,2*dy,dx+2*xoff,dy, ILI9341_RED);
  }
  else {
    tft.fillRect(x0*dx-xoff,2*dy,dx+2*xoff,dy, ILI9341_GREEN);
  }
  //if detect over voltage, same action as above
  if(is_OVER_VOLT) {
    tft.fillRect(x0*dx-xoff,4*dy,dx+2*xoff,dy, ILI9341_RED);
  }
  else {
    tft.fillRect(x0*dx-xoff,4*dy,dx+2*xoff,dy, ILI9341_GREEN);
  }
  //if detect low voltage(low battery), same action as above
  if(is_LOW_VOLT) {
    tft.fillRect(x0*dx-xoff,5*dy,dx+2*xoff,dy, ILI9341_RED);
  }
  else {
    tft.fillRect(x0*dx-xoff,5*dy,dx+2*xoff,dy, ILI9341_GREEN);
  }
  tft.drawRect(x0*dx-xoff,0,dx+2*xoff,dy,ILI9341_BLACK);
  tft.drawRect(x0*dx-xoff,dy,dx+2*xoff,dy,ILI9341_BLACK);
  tft.drawRect(x0*dx-xoff,2*dy,dx+2*xoff,dy,ILI9341_BLACK);
  tft.drawRect(x0*dx-xoff,3*dy,dx+2*xoff,dy,ILI9341_BLACK);
  tft.drawRect(x0*dx-xoff,4*dy,dx+2*xoff,dy,ILI9341_BLACK);
  tft.drawRect(x0*dx-xoff,5*dy,dx+2*xoff,dy,ILI9341_BLACK);

  tft.drawRect(x0*dx-xoff,0,dx+2*xoff,ymax,ILI9341_BLACK);
}

//write the speed of the device
void LCD::writeSpeed(float vel) {
  //this won't happen, but if speed somehow displays over 10mph, keep the
  //speed under 10 (single digits)
  if (vel > 10){
    vel = 9.99;
  }
  //also won't happen, but velocity can't go under 0, for set speed
  if (vel < 0) {
    vel = 0.00;
  }
  //erase current speed reading for new reading
  tft.fillRect(dx-10, 1.7*dy+5, 3*dx, dy, ILI9341_CYAN);
  //write Speed based on input
  uint16_t xSetSpeed = 1.1*dx-10;
  uint16_t ySetSpeed = 2*dy;
  tft.setCursor(xSetSpeed, ySetSpeed);
  tft.setTextColor(ILI9341_BLACK); tft.setTextSize(4);
  tft.println(vel);
}

//write the mode that the state is in to LCD
void LCD::writeMode(State mode) {
  //erase whatever and update
  tft.fillRect(0,4*dy,5.8*dx,1*dy,ILI9341_BLUE);
  if (mode == stateDE) {
    writeDE(); //write Default mode on screen
  }
  else if (mode == stateAA || mode == stateTurnAA) {
    writeAA(); //write Active Assist mode on screen
  }
  else if (mode == stateJS) {
    writeJS(); //write Joystick mode on screen
  }
  else if (mode == stateSS || mode == stateTurnSS) {
    writeSS(); //write Set Speed mode on screen
  }
  else if (mode == stateCE) {
    writeCE(); //write Critical Error mode on screen
  }
  else if (mode == stateSit) {
    writeSitMode(); //write sitting mode
  }
  else {
    writeDE(); //if not in any mode, go to default
  }
}

//write on LCD on if turning
void LCD::writeTurn(js whichTurn) {
  //if turning state is changed, update. otherwise, keep.
  tft.fillRect(0,5*dy,5.8*dx,1*dy,ILI9341_BLUE);
  switch(whichTurn) {
    case jsForward:
      tft.setCursor(35,5*dy+15);
      tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
      tft.println("Forward");
      break;
    case jsRight:
      tft.setCursor(10,5*dy+15);
      tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
      tft.println("Turn Right");
      break;
    case jsLeft:
      tft.setCursor(18,5*dy+15);
      tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
      tft.println("Turn Left");
      break;
    case jsBackward:
      tft.setCursor(30,5*dy+15);
      tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
      tft.println("Backward");
    default:
      tft.setCursor(46,5*dy+15);
      tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
      tft.println("Braked");
      break;
  }
}

void LCD::writeDE() {
  tft.setCursor(40,4.1*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("Default");
}

void LCD::writeAA() {
  tft.setCursor(25,4.4*dy);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Active Assist");
}

void LCD::writeJS() {
  tft.setCursor(35,4.1*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("Joystick");
}

void LCD::writeSS() {
  tft.setCursor(15,4.1*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("Set Speed");
}

void LCD::writeCE() {
  tft.fillRect(0,dy,6*dx,5*dy,ILI9341_BLACK);
  tft.setCursor(10,1.1*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(4);
  tft.println("CRITICAL");
  tft.setCursor(40,2*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(4);
  tft.println("ERROR");
  tft.setCursor(10,3*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Shutting down...");
}

void LCD::writeSitMode() {
  tft.setCursor(30,4.1*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("Sit Mode");
}

void LCD::batteryRemoval() {
  tft.fillRect(0,2.9*dy,6*dx,3*dy,ILI9341_BLACK);
  tft.drawRect(15,3*dy,5*dx,2.8*dy,ILI9341_RED);
  tft.setCursor(20,3*dy+10);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Shutdown");
  tft.setCursor(20,3.5*dy+7);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  tft.println("Complete!");
  tft.setCursor(20,4*dy+15);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("OK TO");
  tft.setCursor(20,4.5*dy+15);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("REMOVE");
  tft.setCursor(20,5*dy+15);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
  tft.println("BATTERY");
}

LCD LCD;
