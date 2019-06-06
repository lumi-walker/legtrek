#include "LCD.h"
#include "state_machine_utils.h"
float speed;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  tft.begin();
  init_interrupts();
  currentState = stateDE;
  prevState = stateDE;
  LCD.loadingScreen();
  init_timer();
  motor_init();
  motor_ready();
  LCD.startScreen();
  Serial.println("ready");

  speed = 1.5;
  M1.setacc(3270);
  M2.setacc(3270);
}

void loop() {
  M1.setspd(speed);
  M2.setspd(speed);
  delay(4000);
  M1.setspd(0);
  M2.setspd(0);
  delay(4000);
}
