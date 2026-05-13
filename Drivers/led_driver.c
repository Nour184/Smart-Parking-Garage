/**
 * @file led_driver.c
 * @brief Hardware Output Configuration - TEAM 1.
 * * TO DO:
 * - Configure GPIO ports/pins for the Red and Green LEDs as Outputs.
 * - Write helper functions like TurnOn_GreenLED(), TurnOff_RedLED(), etc.
 */
#include "led_driver.h"
#define PORTF (1<<5) // bit 5
#define PORTF1 (1<<1) // 
 
void portF_led_init(void){
	SYSCTL_RCGCGPIO_R |= PORTF;
	while((SYSCTL_PRGPIO_R & PORTF)==0){};
	GPIO_PORTF_DIR_R |= 0xE;
	GPIO_PORTF_DEN_R |= 0xE;
}
 
void portF_set_led(LED_COLOR bitmask){
	GPIO_PORTF_DATA_R |= (bitmask & 0xE);
}

void portF_reset_led(void){
	GPIO_PORTF_DATA_R &= ~(0xE);
}
