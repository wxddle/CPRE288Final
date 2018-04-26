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

void detect(){
    //variables
    struct object{
        int distance;
        int width;
        int possibleEndPost;
        int angle;
        int firstAngle;
        int lastAngle;
    };
    struct object objArr[11];
    //Object 0 is dummy
    objArr[0].distance = 1000;
    objArr[0].width = 1000;
    objArr[0].possibleEndPost = 1000;
    int i, j;
    for(i = 1; i<10; i++){
        objArr[i].distance = 0;
        objArr[i].width = 0;
        objArr[i].possibleEndPost = 0;
        objArr[i].angle = 0;
    }

    int objectCount=0, objectStatus=0, firstAngle, lastAngle, degrees, smDist = 0, cyclesDetected = 0;
    double pingDistance, irDistance, currentDistance = 0, firstDistance=0, lastDistance=0, currentSize=0, totalDistance = 0;
    char outString[100];
    for(degrees = 0 ; degrees <= 180 ; degrees += 2){
        servo_move(degrees);
        timer_waitMillis(15);
        pingDistance = (double)(ping_getDistance());
        irDistance = (double)(ir_getDistance());
        if(pingDistance > 100)    {
            pingDistance = 100;
        }
        if(irDistance > 100){
            irDistance = 100;
        }

        //lcd_printf("degrees:%.1lf\nonObj:%d numObj:%d\nir:%.1lf ping:%.1lf\nsize:%.1lf ang:%.1lf\n", degrees, objectStatus, objectCount, pingDistance, pingDistance, smSize, smAng);

        //first edge detection state
        if(pingDistance < 95  && irDistance < 80 && objectStatus != 1){
            firstAngle = degrees;
            currentDistance = pingDistance;
            firstDistance = currentDistance;
            objectStatus = 1;
            //smDist = currentDistance;
            objectCount++;
            totalDistance = pingDistance;
            cyclesDetected++;
        }

        //on object detection state
        else if(pingDistance < 95 && irDistance < 80 && objectStatus == 1){
            currentDistance = pingDistance;
            cyclesDetected++;
            totalDistance += pingDistance;
//            if( currentDistance < smDist || smDist == 0 ){
//                smDist = currentDistance;
//            }
        }

        //last edge detection state
        else if((pingDistance > 95 || irDistance > 80) && objectStatus == 1){
            lastAngle = degrees;
            lastDistance = (firstDistance + smDist) / 2;
            totalDistance = (totalDistance / cyclesDetected);
            currentSize = linearWidth(firstAngle, lastAngle, totalDistance);
            if(currentSize == 1000 || (abs(firstAngle - lastAngle) < 4)){
                objectCount--;
            }
            else{
                objArr[objectCount].width = currentSize;
                objArr[objectCount].distance = totalDistance; //change
                objArr[objectCount].angle = (firstAngle + lastAngle) / 2;
                objArr[objectCount].firstAngle = firstAngle;
                objArr[objectCount].lastAngle = lastAngle;
                if(objArr[objectCount].width <= 5 && objArr[objectCount].width > 1){
                    objArr[objectCount].possibleEndPost = 1;
                }
                else{
                    objArr[objectCount].possibleEndPost = 0;
                }
            }
            //smallest object logic
//          if(currentSize < smSize || (smSize == 0 && currentSize != 1000)){
//                  smSize = currentSize;
//                  smAng = firstAngle - ((firstAngle - lastAngle) / 2);
//                  smObj = objectCount;
//                  smDist = (firstDistance*0.5 + lastDistance*0.5);}

                    //reset current
                    currentDistance = 0;    currentSize = 0;    objectStatus = 0;   totalDistance = 0;         cyclesDetected = 0;
                }
        //send to putty
        int guiDistance;
        if(irDistance > 80){
            guiDistance = 80;
        }
        else{
            guiDistance = irDistance;
        }
        char guiString[81];
        for(j = 0; j<80; j++){
            guiString[j] = ' ';
        }
        for(j = 0; j<guiDistance; j++){
            guiString[j] = 'x';
        }
        sprintf(outString, "%d: %s\n\r", degrees, guiString);
        uart_sendStr(outString);
//        sprintf(outString, "\n\rdegrees: %.1lf\t ir distance: %.2lfcm\t ping distance: %.2lfcm", degrees, pingDistance, pingDistance);
//        uart_sendStr(outString);
    }//end sweeping for loop

    //move to smallest angle and print smallest object details
//    servo_move(smAng);
    //lcd_printf("smallest object: #%d\ndistance: %.1lfcm\nwidth: %.1lfcm", smObj, smDist, smSize);

    //send to putty
    uart_sendStr("\nObjects\n\r");
    for(i = 1; i<11; i++){
        sprintf(outString, "\nDistance: %d\n\rWidth: %d\n\rAngle: %d\n\rFirst Angle: %d\n\rSecond Angle: %d\n\rEnd Post? %d\n\r", objArr[i].distance, objArr[i].width, objArr[i].angle,
                objArr[i].firstAngle, objArr[i].lastAngle, objArr[i].possibleEndPost);
        uart_sendStr(outString);
        if(objArr[i + 1].distance == 0){
            i = 1000;
        }
    }
    servo_move(0);
//    if(sent == 0){
//       sprintf(outString, "\n\n\rsmallest object: #%d\t distance: %.1lfcm\t width: %.1lfcm", smObj, smDist, smSize);
//       uart_sendStr(outString);
//       sent = 1;}
}

//calculate actual linearWidth (bonus method) for smallest object detection
//double linearWidth(double a1, double a2, double d1, double d2){
//    double width = ((d1*0.5+d2*0.5)) * sinf(((a2-a1)*PI)/(360));
//    if(abs(width) < 1)  {return 1000;}
//    return width;
//}

double linearWidth(int angle1, int angle2, double distance) {
    int objectAngle = ((angle2 - angle1));
    float angleInRads = ((float)objectAngle * 3.1415)/180.0;

    float width = distance * sinf(angleInRads/2.0);

    if (width < 0) width *= -1;
    if(width < 1) width = 1000;
    return width;
}

void explore(){

    oi_t *sensor_data = oi_alloc();
    //oi_free(sensor_data);
    oi_init(sensor_data);


    char input;

    input = uart_receive();
    //forwards
    if (input == 'w')
    {
        char moveForward[] = "Moving forward...\r\n";
        uart_sendStr(moveForward);
        move_forward(sensor_data, 100); //move forward

    }
    //backwards
    else if (input == 's')
    {
        char moveBackwards[] = "Moving Backwards...\r\n";
        uart_sendStr(moveBackwards) ;
        move_backwards(sensor_data, -100); //backwards
    }

    //rotates clockwise
    else if (input == 'd')
    {
        char turnRight[] = "Rotating RIGHT 90 degrees\r\n";
        uart_sendStr(turnRight);
        turn_clockwise(sensor_data, M_PIturn/2.0) ;//rotate 90 degrees

    }

    //rotates counterclockwise
    else if (input == 'a')
    {
        char turnLeft[] = "Rotating LEFT 90 degrees\r\n";
        uart_sendStr(turnLeft) ;
        turn_counterClockwise(sensor_data, M_PIturn/2.0) ;
    }

    else if (input == 'e')
    {
        char turnRight[] = "Rotating RIGHT 30 degrees\r\n";
        uart_sendStr(turnRight);
        turn_clockwise(sensor_data, M_PIturn/6.0) ;//rotate 90 degrees

    }
    else if (input == 'q')
    {
        char turnLeft[] = "Rotating LEFT 30 degrees\r\n";
        uart_sendStr(turnLeft) ;
        turn_counterClockwise(sensor_data, M_PIturn/6.0) ;
    }

    //scan surroundings
    else if (input == 'o')
    {
        char scanning[] = "Scanning..... please wait......\r\n";
        uart_sendStr(scanning);
        detect() ;
    }

    //play song
    else if (input == 'p')
    {
        char songStatement[] = "Playing Song\r\n";
        uart_sendStr(songStatement) ;

        unsigned char notes[36] = {67,66,67,64,65,66,67,72,76,76,74,76,77,71,74,72};
        unsigned char lengths[36] = {17,17,17,102,51,51,102,51,38,13,51,51,51,51,102,51};
        oi_loadSong(0, 16, notes, lengths);

        oi_play_song(0);
     //   oi_free(sensor_data);

    }
    else if (input == 'z')
    {
        char turnLeft[] = "Rotating LEFT 180 degrees\r\n";
        uart_sendStr(turnLeft) ;
        turn_counterClockwise(sensor_data, M_PIturn);
    }
    else if (input == 'c')
    {
        char turnRight[] = "Rotating RIGHT 180 degrees\r\n";
        uart_sendStr(turnRight);
        turn_clockwise(sensor_data, M_PIturn) ;

    }
    else if (input == '1')
    {
        char turnLeft[] = "Rotating LEFT 10 degrees\r\n";
        uart_sendStr(turnLeft) ;
        turn_counterClockwise(sensor_data, M_PIturn/18.0);
    }
    else if (input == '3')
        {
            char turnRight[] = "Rotating RIGHT 180 degrees\r\n";
            uart_sendStr(turnRight);
            turn_clockwise(sensor_data, M_PIturn/18.0) ;

        }
    else if (input == '`'){
        uart_sendStr("Flashing LED");
        while(1){
            oi_free(sensor_data);
        }
    }
    //move_forward(sensor_data,100);


    oi_free(sensor_data);
}

void move_backwards(oi_t *sensor, int centimeters)
{
    oi_setWheels(-100, -100); //move backward; half speed

    int sum = 0;

    while (sum > centimeters)
    {
        oi_update(sensor);

        sum += sensor->distance;
        timer_waitMillis(1);

    }
    sum = 1000;
    oi_setWheels(0,0); //stop wheels

}

/**
*   This method will be used to rotate the Roomba counterClockwise a given amount of degrees.
*   @author Brian Bradford, Nick Knuth, Andrew Thai, and Rajiv Bhoopala
*   @param  *sensor     Pointer to struct containing Roomba sensor data
*   @param  degrees     Degrees for Roomba to rotate
*   @date 4/12/2017
*/
void turn_counterClockwise(oi_t *sensor, float degrees){
    oi_setWheels(100, -100);
    timer_waitMillis(degrees*22);
    oi_setWheels(0, 0); //stop wheels

}

/**
*   This method will be used to rotate the Roomba clockwise a given amount of degrees.
*   @author Brian Bradford, Nick Knuth, Andrew Thai, and Rajiv Bhoopala
*   @param  *sensor     Pointer to struct containing Roomba sensor data
*   @param  degrees     Degrees for Roomba to rotate
*   @date 4/12/2017
*/
void turn_clockwise(oi_t *sensor, float degrees){
    oi_setWheels(-100, 100);
    timer_waitMillis(degrees*22);
    oi_setWheels(0, 0); //stop wheels
}



void move_forward(oi_t *sensor, int centimeters){
    int sum = 0 ;
    char output[30];

    while (sum < centimeters)
    {
        int leftSignal = sensor -> cliffLeftSignal;
        int rightSignal = sensor -> cliffRightSignal;
        int frontLeftSignal = sensor -> cliffFrontLeftSignal;
        int frontRightSignal = sensor -> cliffFrontRightSignal;
        //int actualDistance = sensor -> distance;
        oi_setWheels(70,90) ; //right,left
        oi_update(sensor);
        sum += sensor->distance;



        //White line (boundary) detection
        if(leftSignal > 2600)
        {
            char* warning = "White Line on Left Side! \n\r";
            uart_sendStr(warning);
            lcd_printf("actDist: %d", sum);
            sprintf(output, "\nactDist: %d\n\r", sum);
            uart_sendStr(output);
            oi_setWheels(0,0);
            //sum = 1000;
            break;
        }
      if(rightSignal > 2600)
        {
            char* warning = "White Line on RIGHT side! \n\r";
            uart_sendStr(warning);
            lcd_printf("actDist: %d", sum);
            oi_setWheels(0,0);
            //sum = 1000;
            break ;
        }
      if(frontLeftSignal > 2600)
        {
            char* warning = "White Line on Front Left side! \n\r";
            uart_sendStr(warning);
            oi_setWheels(0,0);
            lcd_printf("actDist: %d", sum);
            //sum = 1000;
            break ;
        }
      if(frontRightSignal > 2600)
        {
            char* warning = "White Line on Front Right side! \n\r";
            uart_sendStr(warning);
            oi_setWheels(0,0);
            lcd_printf("actDist: %d", sum);
            //sum = 1000;
            break ;
        }

        //Bumper hit object detection
        //if (checkBumper(sensor) == 1)
        if(sensor -> bumpRight)
        {
            char* warning = "Hit right bumper! \n\r";
            uart_sendStr(warning);
            oi_setWheels(0,0);
            break ;
        }

        if (sensor -> bumpLeft)
        {
            char* warning = "Hit left bumper! \n\r";
            uart_sendStr(warning);
            oi_setWheels(0,0);
            break;
        }

        //Cliff detection check
        if (checkCliff(sensor) == 1)
        {
            oi_setWheels(0,0);
            char* warning = "WARNING CLIFF ON LEFT SIDE! \n\r";
            uart_sendStr(warning);

            break;
        }
        if (checkCliff(sensor) ==2)
        {
            oi_setWheels(0,0);
            char* warning = "WARNING CLIFF ON RIGHT SIDE! \n\r";
            uart_sendStr(warning);

            break;
        }
        if (checkCliff(sensor) ==3)
        {
            oi_setWheels(0,0);
            char* warning = "WARNING CLIFF IN FRONT ON LEFT SIDE! \n\r";
            uart_sendStr(warning);

            break;
        }
        if (checkCliff(sensor) == 4)
        {
            oi_setWheels(0,0);
            char* warning = "WARNING CLIFF IN FRONT ON RIGHT SIDE \n\r";
            uart_sendStr(warning);
            break;
        }
        timer_waitMillis(100);
        //lcd_printf("actDist: %d", actualDistance);
    }
    lcd_printf("actDist: %d", sum);
    sprintf(output, "\nactDist: %d\n\r", sum);
    uart_sendStr(output);
    oi_setWheels(0,0); //stop wheels
}

int checkBumper(oi_t *sensor)
{
    /**
     * Bumper States:
     * bumper = 0, no bumps
     * bumper = 1, right bumper
     * bumper = 2, front right bumper
     * bumper = 3, center right bumper
     * bumper = 4, center left bumper
     * bumper = 5, front left bumper
     * bumper = 6, left bumper
     */
    int bumper = 0;

    oi_update(sensor);
    if (sensor-> bumpLeft)
    {
        bumper = 6;
    }

    else if (sensor-> bumpRight)
    {
        bumper = 1;
    }
    else
    {
        bumper = 0;
    }

    return bumper;
}

/**
*   This method will be used to check the state of the cliff sensors.
*   @author Brian Bradford, Nick Knuth, Andrew Thai, and Rajiv Bhoopala
*   @param  *sensor     Pointer to struct containing Roomba sensor data
*   @date 4/12/2017
*/
int checkCliff(oi_t *sensor)
{
    oi_update(sensor);

    /**
     *  0 = no cliff
     *  1 = cliffleft
     *  2 = cliffright
     *  3 = clifffront left
     *  4 = clifffront right
     */
    int cliff = 0;

    if(sensor-> cliffLeft)
    {
        cliff = 1;
    }
    else if(sensor -> cliffRight)
    {
        cliff = 2;
    }
    else if(sensor -> cliffFrontLeft)
    {
        cliff = 3;
    }
    else if (sensor -> cliffFrontRight)
    {
        cliff = 4;
    }

    else
    {
        cliff = 0;
    }

    return cliff;
}