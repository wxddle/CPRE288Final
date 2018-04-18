#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#define PI 3.14159265

#include "open_interface.h"

double linearWidth(double a1, double a2, double d1, double d2);
void detect();
void explore();

void move_forward(oi_t *sensor, int centimeters);
void move_backwards(oi_t *sensor, int centimeters);
void turn_counterClockwise(oi_t *sensor, int degrees);
void turn_clockwise(oi_t *sensor, int degrees);

int cliffCheck();
int bumpCheck();
int lineCheck();

#endif /* FUNCTIONS_H_ */
