
#include "ui_state_machine.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //tft.begin();
  //LCD.loadingScreen();
  //LCD.startScreen();
}
//int prevBatt = 0;
void loop() {
  // put your main code here, to run repeatedly:
  runStateMachine(currState);
  Serial.print("Speed: ");
  Serial.println(setSpeed);
  delay(500);
  /*
   State MODE = stateCE;
   if (prevState != MODE) {
      LCD.writeMode(MODE);
      prevState = MODE;
      LCD.batteryRemoval();
   }
   
   js TURN = jsLeft;
   if (prevTurn != TURN) {
      LCD.writeTurn(TURN);
      prevTurn = TURN;
   }
   if(millis() >= 6000 && millis() < 6100) {
    is_FAULTY_CURRENT_SENSOR = 1;
    is_OVER_CURRENT = 1;
    LCD.writeErrorPanel();
    low_batt = 1;
    batterylvl = 8;
   }
   
   if (prevBatt != batterylvl || batterylvl < 10) {
    LCD.writeBatteryLevel(batterylvl);
    prevBatt = batterylvl;
   }*/
}
