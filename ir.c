#include "lcd.h"
#include "ir.h"
#include <inc\tm4c123gh6pm.h>
#include <math.h>

void ir_init() {
	//enableADC 0 module on portD
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
    //enable clock for ADC
    SYSCTL_RCGCADC_R |= 0x1;
    //enable port D pin 0 to work as alternate functions
    GPIO_PORTD_AFSEL_R |= 0x01;
    //set pin to input -0
    GPIO_PORTD_DEN_R &= 0b11111110;
    //disable analog isolation for the pin
    GPIO_PORTD_AMSEL_R |= 0x01;
    //initialize the port trigger source as processor (default)
    GPIO_PORTD_ADCCTL_R = 0x00;

    //disable SS0 sample sequencer to configure it
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN0;
    //initialize the ADC trigger source as processor (default)
    ADC0_EMUX_R = ADC_EMUX_EM0_PROCESSOR;
    //set 1st sample to use the AIN10 ADC pin
    ADC0_SSMUX0_R |= 0x000A;
    //enable raw interrupt status
    ADC0_SSCTL0_R |= (ADC_SSCTL0_IE0 | ADC_SSCTL0_END0);
    //enable oversampling to average
    ADC0_SAC_R |= ADC_SAC_AVG_64X;
    //re-enable ADC0 SS0
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN0;
}

int ir_read(void) {
    //initiate SS1 conversion
    ADC0_PSSI_R=ADC_PSSI_SS0;
    //wait for ADC conversion to be complete
    while((ADC0_RIS_R & ADC_RIS_INR0) == 0){//wait

    }
        //grab result
    int value = ADC0_SSFIFO0_R;
    return value;
}

double average() {                  //sample averaging function
    int total = 0;
    int i = 0;

    for (i = 0; i < 16; i++) {
        ir_read();
        total = total + ir_read();
        //timer_waitMillis(50);       //measure every 50 ms
    }

    total = total/16;               //take the average value after 800 ms (50 ms * 16)
    return total;
}

float ir_getDistance(){
	float i = 0.0;
	float total = 0.0;
	//Compute a running average of 16 samples and display that value
	for (i = 0; i < 16; i++) {
	        ir_read();
	        total = total + ir_read();
	        timer_waitMillis(10);       //measure every 50 ms
	}
	float q = total/16.0;
	//equation from excel y = 83327 x ^(-1.129) where y = distance (cm) and x = quantized value
	float distance = 54209*pow(q, -1.087);
	return distance;
}
