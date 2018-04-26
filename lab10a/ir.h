#ifndef IR_H_
#define IR_H_


// Initialize the IR distance sensor
void ir_init(void);

//read the raw ADC values from the IR sensor
int ir_read(void);

float ir_getDistance();

volatile unsigned isr_value;

#endif /* IR_H_ */
