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
//void print_left(oi_t *sensor) { //comment out when not testing cliff sensor print
//    oi_t *sensor_data = oi_alloc();
//    oi_init(sensor_data);
//    int leftSignal = sensor -> cliffLeftSignal;
//    lcd_printf("%d", leftSignal);
//}

void main(void)
{
    //initialize everything
    uart_init();
    lcd_init();
    ping_init();
    servo_init();
    ir_init();
    init_map();

    timer_waitMillis(1000);



    //sweep 180 degrees and detect objects using method from functions.c
    //detect();
    oi_t *sensor_data = oi_alloc();
    //oi_free(sensor_data);
    oi_init(sensor_data);

//    oi_t *sensor_data = oi_alloc(); //comment out when not testing cliff sensor print
//    oi_init(sensor_data);           //comment out when not testing cliff sensor print
//    oi_free(sensor_data);
//
    while (5 > 4){
////
////        print_left(sensor_data);    //comment out when not testing cliff sensor print
////       timer_waitMillis(500);

      explore();
    }

}
