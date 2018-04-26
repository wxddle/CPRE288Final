/**
*
*   @file  uart.c
*
*
*
*
*
*   @author
*   @date
*/

#include "uart.h"

/**
 * @brief sets all necessary registers to enable the uart 1 module.
 */
void uart_init(void){
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1; //enable clock to GPIO, port B
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R1; //enable clock to UART1
    GPIO_PORTB_AFSEL_R |= (BIT0 | BIT1); //enable alternate functions
    GPIO_PORTB_PCTL_R |= 0x00000011; //enable rx and tx on port b pins 0 and 1
    GPIO_PORTB_DEN_R |= (BIT0 | BIT1); //set pins 0 and 1 to digital
    GPIO_PORTB_DIR_R &= ~ BIT0; //SET PIN 0 TO INPUT
    GPIO_PORTB_DIR_R |= BIT1; //SET PIN 1 TO OUTPUT
    uint16_t iBRD = 8; //baudrate
    uint16_t fBRD = 44;
    UART1_CTL_R &= ~(UART_CTL_UARTEN); //TURN OFF UART FOR SETUP
    UART1_IBRD_R = iBRD; //SET BAUD RATE
    UART1_FBRD_R = fBRD; //SET BAUD RATE
    UART1_LCRH_R = UART_LCRH_WLEN_8; //SET FRAME, 8 DATA BITS, 1 STOP BIT, NO PARITY, NO FIFO
    UART1_CC_R = UART_CC_CS_SYSCLK; //USE SYSTEM CLOCK AS SOURCE
    UART1_CTL_R = (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN); //RE-ENABLE output, input, and uart1

}

/**
 * @brief Sends a single 8 bit character over the uart 1 module.
 * @param data the data to be sent out over uart 1
 */
void uart_sendChar(char data){
    while(UART1_FR_R & 0x20) //wait until there is room to send data
    {
    }

    UART1_DR_R = data; //send data
}

/**
 * @brief polling receive an 8 bit character over uart 1 module.
 * @return the character received or a -1 if error occured
 */
int uart_receive(void){
    char data = 0;

    if(UART1_FR_R & UART_FR_RXFE) //wait to receive
    {
        return '$';
    }

    data = (char) (UART1_DR_R & 0xFF);

    return data;

}

/**
 * @brief sends an entire string of character over uart 1 module
 * @param data pointer to the first index of the string to be sent
 */
void uart_sendStr(const char *data){
    while(*data != '\0')
    {
        uart_sendChar(*(data++));
    }
}
