#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "open_interface.h"

#define PI 3.14159265

void detect();

double linearWidth(double a1, double a2, double d1, double d2);

void move_forward(oi_t *sensor, int centimeters);

int checkBumper(oi_t *sensor);

int checkCliff(oi_t *sensor);

void explore();

void move_backwards(oi_t *sensor, int centimeters);

void turn_counterClockwise(oi_t *sensor, int degrees);

void turn_clockwise(oi_t *sensor, int degrees);


#endif /* FUNCTIONS_H_ */
