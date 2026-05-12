#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "TM4C123GH6PM.h"

void portF_init(void);
void portF_set_led(uint8_t bit_mask);
void portF_reset_led(void);

#endif