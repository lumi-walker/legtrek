#ifndef UI_UTILS_H
#define UI_UTILS_H

/*
	Contains parameters and functions related to the UI
*/


// SET SPEED PARAMS
float dSpeed = 0.1; //speed change


//JOYSTICK PARAMS
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

#endif