#ifndef MOTOR_CALIB_CONS_H
#define MOTOR_CALIB_CONS_H

float DEFAULT_ACCEL = 2000.0f;
bool FORWARD = true;
bool REVERSE = false;

//speed calibration (range .55V-2.75V)
float minSpeed = 0.3; //minimum limit in set speed
float maxSpeed = 1.5; //maximum limit in set speed
//acc calibration (range 0V-5V)
int min_acc_rpmps = 0;
int max_acc_rpmps = 3270;

//setting for MUX
bool GROUND = 0;
bool DAC = 1;

//for motor OUTPUT reading
bool R_RUN = 0;
#endif
