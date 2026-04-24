#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include "tm4c123gh6pm.h"
#include "stdint.h"

void UART0_Init(void);
void UART0_SendChar(uint8_t data);
uint8_t UART0_ReceiveChar(void);

#endif