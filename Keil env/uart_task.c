#include "uart_task.h"

void uartTask(void *pvParameters){
	char *stringToPrint = NULL;
	while(1){
		xQueueReceive(printQueue, (void *)&stringToPrint, portMAX_DELAY);
		UART0_send_string(stringToPrint);
	}
}