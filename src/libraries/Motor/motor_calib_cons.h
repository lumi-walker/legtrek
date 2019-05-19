#ifndef MOTOR_CALIB_CONS_H
#define MOTOR_CALIB_CONS_H

float DEFAULT_ACCEL = 5000.0f;
bool FORWARD = true;
bool REVERSE = false;

//speed & acc calibration
int max_acc_rpmps = 4204;
int min_acc_rpmps = 8315;
float minSpeed = 0.3; //minimum limit in set speed
float maxSpeed = 1.5; //maximum limit in set speed

//setting for MUX
bool GROUND = 0;
bool DAC = 1;

//for motor OUTPUT reading
bool R_RUN = 0;
#endif
