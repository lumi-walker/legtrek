#include "state_machine_utils.h"
//float speed_sp;
bool M1direct = FORWARD;
bool M2direct = FORWARD;
void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
  tft.begin();
  init_interrupts();
  currentState = stateDE;
  prevState = stateDE;
  LCD.loadingScreen();
  init_timer();
  init_motor();
  init_BMS();
  LCD.startScreen();
  relayController.connect();
  Serial.println("ready");
  
  speed_sp = 1.2;
  M1.setacc(3270);
  M2.setacc(3270);
}

void loop() {
  faststopoff_all();
  M1direct = FORWARD;
  M2direct = FORWARD;
  M1.setdirect(M1direct);
  M2.setdirect(M2direct);
  M1.setspd(speed_sp);
  M2.setspd(speed_sp);
  delay(1000);
  
  faststopon_all();
  delay(1000);
faststopoff_all();
  M1direct = REVERSE;
  M2direct = REVERSE;
  M1.setdirect(M1direct);
  M2.setdirect(M2direct);
  M1.setspd(speed_sp);
  M2.setspd(speed_sp);
  delay(1000);
  faststopon_all();
//  M1.setspd(0);
//  M2.setspd(0);
  delay(1000);
  Serial.println("error" + String(isMotorCorrect()));
}
