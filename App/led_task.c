#include "led_task.h"
#include "led_driver.h"

void LedTask(void *pvParamters){
  Led_t evLed = NULL;
	while(1){
		 xQueueReceive(ledQueue, (void *)&evLed, portMAX_DELAY);
		 
		 switch(evLed){
			case EV_SET_GREEN:
					portF_set_led(GREEN_LED);
					break;
			case EV_SET_RED:
					portF_set_led(RED_LED);
					break;
			case EV_RESET_LED:
					portF_reset_led();
					break;
			}
	}
}
