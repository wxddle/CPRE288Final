#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "open_interface.h"

#define PI 3.14159265
//#define M_PIturn 138.0 //CyBot 2 - BAD IR SENSOR
#define M_PIturn 146.0 //CyBot 6
void detect();

double linearWidth(int angle1, int angle2, double distance);

void move_forward(oi_t *sensor, int centimeters);

int checkBumper(oi_t *sensor);

int checkCliff(oi_t *sensor);

void explore();

void move_backwards(oi_t *sensor, int centimeters);

void turn_counterClockwise(oi_t *sensor, float degrees);

void turn_clockwise(oi_t *sensor, float degrees);


#endif /* FUNCTIONS_H_ */