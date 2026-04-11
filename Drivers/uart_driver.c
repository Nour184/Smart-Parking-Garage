/*
this file is used just for testing on the tiva before integration
*/

#include "uart_driver.h"

void UART0_Init(void) {
    SYSCTL_RCGCUART_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x01;
    
    while((SYSCTL_PRUART_R & 0x01) == 0){};
    while((SYSCTL_PRGPIO_R & 0x01) == 0){};
    UART0_CTL_R &= ~0x01;
    
    // --- START WITH 16 MHz VALUES ---
//    UART0_IBRD_R = 8;
//    UART0_FBRD_R = 44;
    
    // IF PYTHON STILL PRINTS '0xfc', COMMENT OUT THE TWO LINES 
    // ABOVE AND UNCOMMENT THESE 50 MHz VALUES:
     UART0_IBRD_R = 27;
     UART0_FBRD_R = 8;
    
    UART0_LCRH_R = 0x70;
    UART0_CC_R = 0x0;
    
    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_DEN_R |= 0x03;
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0x00000011;
    GPIO_PORTA_AMSEL_R &= ~0x03;
    
    UART0_CTL_R |= 0x301;
}

void UART0_SendChar(uint8_t data) {
    while((UART0_FR_R & 0x20) != 0);
    UART0_DR_R = data;
}


uint8_t UART0_ReceiveChar(void) {
    // Wait while the Receive FIFO is Empty (RXFE bit is 1)
    while((UART0_FR_R & 0x10) != 0);
    return (uint8_t)(UART0_DR_R & 0xFF);
}