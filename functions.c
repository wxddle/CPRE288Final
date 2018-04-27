/*
 *      Authors: Jay Amin & Tilden Chen
 *      Class: functions.c
 *      Purpose: holder class for bigger functions for main to use
 */

//actually using includes
#include "functions.h"
#include "servo.h"
#include "ping.h"
#include "ir.h"
#include "lcd.h"
#include "uart.h"
#include <math.h>
#include "open_interface.h"

//just in case includes
#include "timer.h"
#include <stdint.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"

//detect function for 180 degree object detection. Modify this function change object awareness and detection algorithms
void detect(){
    //variables
    int objectCount=0, objectStatus=0, smObj=0, sent=0;
    double degrees, pingDistance, irDistance, firstAngle, lastAngle, currentDistance = 0, firstDistance=0, lastDistance=0, currentSize=0, smAng=0, smSize=0, smDist=0;
    char outString[100];

    for(degrees = 0 ; degrees <= 180 ; degrees += 2){
        servo_move(degrees);
        pingDistance = (double)(ping_getDistance());
        irDistance = (double)(ir_getDistance());
        if(irDistance > 150)    {irDistance = 150;}
        if(pingDistance > 350)  {pingDistance = 350;}

        //first edge detection state
        if(irDistance < 80 && pingDistance < 80 && objectStatus != 1){
            firstAngle = degrees;
            currentDistance = irDistance;   firstDistance = currentDistance;
            objectStatus = 1;               objectCount++;}

        //on object detection state
        else if(irDistance < 80 && abs(irDistance - currentDistance) < 15 && objectStatus == 1){
                       currentDistance = irDistance;}

        //last edge detection state
        else if(irDistance > 80 && objectStatus == 1){
            lastDistance = currentDistance;
            lastAngle = degrees;
            currentSize = linearWidth(firstAngle, lastAngle, firstDistance, lastDistance);
            if(currentSize == 1000)         {objectCount--;}

            //smallest object logic
            if(currentSize < smSize || (smSize == 0 && currentSize != 1000)){
                smSize = currentSize;
                smAng = firstAngle - ((firstAngle - lastAngle) / 2);
                smObj = objectCount;
                smDist = (firstDistance*0.5 + lastDistance*0.5);}

            //reset current
            currentDistance = 0;    currentSize = 0;    objectStatus = 0;}

        //send to putty
        sprintf(outString, "\n\rdegrees: %.1lf\t ir distance: %.2lfcm\t ping distance: %.2lfcm", degrees, irDistance, pingDistance);
        uart_sendStr(outString);
    }//end sweeping for loop

    //move to smallest angle and print smallest object details
    servo_move(smAng);
    //lcd_printf("smallest object: #%d\ndistance: %.1lfcm\nwidth: %.1lfcm", smObj, smDist, smSize);

    //send to putty
    if(sent == 0){
       sprintf(outString, "\n\n\rsmallest object: #%d\t distance: %.1lfcm\t width: %.1lfcm", smObj, smDist, smSize);
       uart_sendStr(outString);
       sent = 1;}
}

//calculate actual linearWidth (bonus method) for smallest object detection
double linearWidth(double a1, double a2, double d1, double d2){
    double width = ((d1*0.5+d2*0.5)) * tanf(((a2-a1)*PI)/(360));
    if(abs(width) < 1)  {return 1000;}
    return width;
}

//explore function which will be looped forever to receive input, relay and react to sensor information, and report back feedback based on user commands
void explore(){

    //initializing sensor_data
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    //placeholder variable for user input received from putty ; change if we use an alternative contorller
    char command = uart_receive();

    if (command == 'w'){
        char response[] = "Attempting to move forward 25cm\r\n";
        uart_sendStr(response);
        move_forward(sensor_data, 25);
    }

    else if (command == 's'){
        char response[] = "Moving backwards 100cm\r\n";
        uart_sendStr(response) ;
        move_backwards(sensor_data, -100);
    }

    else if (command == 'd'){
        char response[] = "Rotating CW 10°\r\n";
        uart_sendStr(response);
        turn_clockwise(sensor_data, 5);
    }

    else if (command == 'a'){
        char response[] = "Rotating CCW 10°\r\n";
        uart_sendStr(response) ;
        turn_counterClockwise(sensor_data, 5);
    }

    else if (command == 'o'){
        char response[] = "Attempting to detect debris\r\n";
        uart_sendStr(response);
        detect();
    }
    else {				//NEW CODE, SENDS $ while waiting
	char waiting[] = "$\r\n"
	uart_sendStr(waiting);
	timer_waitMillis(1000);


//    else if (command == 'p'){
//        char response[] = "Playing SOS beacon\r\n";
//        uart_sendStr(response) ;
//        char pitch[18] = {66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66};
//        char length[18] = {15,15,15,45,45,45,15,15,15,45,45,45,15,15,15,45,45,45};
//        oi_loadSong(0, 20, pitch, length);
//        oi_play_song(0);}

    oi_free(sensor_data);
}

void move_forward(oi_t *sensor, int centimeters){
    int counter = 0;

    while (counter < centimeters){
        oi_setWheels(75,75);
        oi_update(sensor);
        counter += sensor->distance;

        if (cliffCheck() == 1) {
			break;
		}
        else if (bumpCheck() == 1) {
			break;
		}
        else if (lineCheck() == 1) {
			break;
		}

        timer_waitMillis(100);
    }

    oi_setWheels(0,0);
    oi_free(sensor);
}

void move_backwards(oi_t *sensor, int centimeters){
    oi_setWheels(-100, -100);
    int counter = 0 ;

    while (counter > centimeters){
        oi_update(sensor);
        counter += sensor->distance;
        timer_waitMillis(1);}

    oi_setWheels(0,0);
    oi_free(sensor);
}

void turn_counterClockwise(oi_t *sensor, int degrees){
    oi_setWheels(100, -100);
    timer_waitMillis(100);
    oi_setWheels(0, 0);
    oi_free(sensor) ;
}

void turn_clockwise(oi_t *sensor, int degrees){
    oi_setWheels(-100, 100);
    timer_waitMillis(100);
    oi_setWheels(0, 0);
    oi_free(sensor) ;
}

int lineCheck(oi_t *sensor, int centimeters){
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    int lineThreshold = 2500;


    if(sensor -> cliffLeftSignal > lineThreshold){
        char* warning = "White line on the left!\n\r";
        oi_setWheels(0,0);
        uart_sendStr(warning);
        return 1;
    }

    if(sensor -> cliffRightSignal > lineThreshold){
        char* warning = "White line on the right!\n\r";
        oi_setWheels(0,0);
        uart_sendStr(warning);
        return 1;
    }

    if(sensor -> cliffFrontLeftSignal > lineThreshold){
        char* warning = "White line on the front left!\n\r";
        oi_setWheels(0,0);
        uart_sendStr(warning);
        return 1;
    }

    if(sensor -> cliffFrontRightSignal > lineThreshold){
        char* warning = "White line on the front right!\n\r";
        oi_setWheels(0,0);
        uart_sendStr(warning);
        return 1;
    }
    else {
        return 0;
    }
}

int bumpCheck(oi_t *sensor, int centimeters){

    if(sensor -> bumpRight){
        char* warning = "Hit right bumper! \n\r";
        oi_setWheels(0,0);
        uart_sendStr(warning);
      
        return 1;
    }

    if (sensor -> bumpLeft) {
        char* warning = "Hit left bumper! \n\r";
        oi_setWheels(0,0);
        uart_sendStr(warning);
      
        return 1;
    }
    else {
        return 0;
    }
}

int cliffCheck(oi_t *sensor, int centimeters){


    if (sensor-> cliffLeft){
        oi_setWheels(0,0);
        char* warning = "Sinkhole on the left!\n\r";
        uart_sendStr(warning);
        return 1;
    }

    if (sensor -> cliffRight){
        oi_setWheels(0,0);
        char* warning = "Sinkhole on the right!\n\r";
        uart_sendStr(warning);
        return 1;
    }

  if (sensor -> cliffFrontLeft){
      oi_setWheels(0,0);
      char* warning = "Sinkhole on front left!\n\r";
      uart_sendStr(warning);
      return 1;
	  }

  if (sensor -> cliffFrontRight){
      oi_setWheels(0,0);
      char* warning = "Sinkhole on front right!\n\r";
      uart_sendStr(warning);
      return 1;
	  }
  else {
      return 0;
  }
}
