/*
 * servo.h
 *
 *  Created on: Mar 29, 2018
 *      Author: jayamin
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include "timer.h"


//pwm value based on period, clock (16MHz), and 20ms cycle
volatile signed PWM;

//initialization
void servo_init();

//move to specific degree based on manually calibrated values and PWM
void servo_move(double degree);

#endif /* SERVO_H_ */
