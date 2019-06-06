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
}

void loop() {
  speed = 1;
  M1.setacc(DEFAULT_ACCEL/2);
  M2.setacc(DEFAULT_ACCEL);
  M1.setspd(speed/2);
  M2.setspd(speed);
  delay(4000);
  speed = 0;
  M1.setspd(speed);
  M2.setspd(speed);
  delay(4000);
}
