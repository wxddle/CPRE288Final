/*
 * servo.c
 *
 *  Created on: Mar 29, 2018
 *      Author: jayamin
 */

#include "servo.h"


void servo_init(){
    //initializing GPIO by enabling, setting outputs, etc
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
    GPIO_PORTB_AFSEL_R |= BIT5;
    GPIO_PORTB_PCTL_R |= 0x700000;
    GPIO_PORTB_DIR_R |= BIT5;
    GPIO_PORTB_DEN_R |= BIT5;

    //turning on timer 1 clock and disabling it during configuration
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    TIMER1_CTL_R &= ~(TIMER_CTL_TBEN);
    TIMER1_CFG_R |= TIMER_CFG_16_BIT;

    //enabling pwm and configuring it
    TIMER1_TBMR_R |= (TIMER_TBMR_TBMR_PERIOD | TIMER_TBMR_TBAMS);
    TIMER1_TBMR_R &= ~(TIMER_TBMR_TBCMR);
    TIMER1_CTL_R &= ~(TIMER_CTL_TBPWML);
    TIMER1_TBILR_R |= (320000 & 0xFFFF);
    TIMER1_TBPR_R |= (320000 >> 16);

    //assigning volatile signed PWM variable to zero to init and setting up PWM in timer 1
    PWM = 0;
    servo_move(PWM);
    TIMER1_TBMATCHR_R |= ((320000 - PWM) & 0xFFFF);
    TIMER1_TBPMR_R |= ((320000- PWM) >> 16);

    //enabling timer 1 making it ready for use
    TIMER1_CTL_R |= TIMER_CTL_TBEN;
}

void servo_move(double degree){
    //calibration values based on PWM and degrees and Roomba #cyBOT 26
    PWM = ((163*degree)+475);

    //SETTING 24 BITS OF TIMER 1 (also configured above, needs to be re-evaluated actively based on degree)
    //match value
    TIMER1_TBMATCHR_R = ((320000 - PWM) & 0xFFFF);
    //start value
    TIMER1_TBPMR_R |= ((320000 - PWM) >> 16);
}
