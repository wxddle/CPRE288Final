/*
 *      Authors: Jay Amin & Tilden Chen
 *      Class: main.c
 *      Purpose: main interface to run code and helper methods from
 */

#include "lcd.h"
#include "ping.h"
#include "ir.h"
#include "functions.h"
#include "servo.h"
#include "uart.h"
#include "open_interface.h"

/*
 *      Main method with inits and detect/sweeper function from functions.c
 */
void main(void){
    //initialize everything
    uart_init();
    lcd_init();
    ping_init();
    servo_init();
    ir_init();

    timer_waitMillis(1000);

    //sweep 180 degrees and detect objects using method from functions.c
    //detect();

    while (5 > 4){
        explore();
    }

}
