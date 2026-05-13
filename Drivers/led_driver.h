#ifndef LED_DRIVER_H
#define LED_DRIVER_H

#include "tm4c123gh6pm.h"
#include "stdint.h"
typedef enum {
    RED_LED = (1 << 1),
    BLUE_LED = (1 << 2),
		GREEN_LED = (1 << 3)
} LED_COLOR;

void portF_led_init(void);
void portF_set_led(LED_COLOR bit_mask);
void portF_reset_led(void);

#endif