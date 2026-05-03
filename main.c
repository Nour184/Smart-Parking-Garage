# include "FreeRTOS.h"
# include "task.h"
# include "basic_io.h"
# include "gpio_driver.h"
# include "event.h"
# include "Input_task.h"
#include "queue.h"
# include "Receive_task.h"
#include "button_event.h"
# include "ledTask.h"
QueueHandle_t xGateEventQueue;
QueueHandle_t xLEDEventQueue;
QueueHandle_t xButtonInterruptQueue;
void vTestReceiverTask(void *pvParameters);

int main(void)
{
    GPIO_AllInit();

xGateEventQueue = xQueueCreate(20, sizeof(GateEvent_t));
xLEDEventQueue = xQueueCreate(20, sizeof(GateEvent_t));
xButtonInterruptQueue = xQueueCreate(20, sizeof(ButtonId_t));

if (xGateEventQueue == NULL ||
    xLEDEventQueue == NULL ||
    xButtonInterruptQueue == NULL)
{
    while (1);
}

InputTask_Init();

xTaskCreate(vInputTask, "Input", 256, NULL, 4, NULL);
xTaskCreate(vTestReceiverTask, "Gate", 256, NULL, 3, NULL);
xTaskCreate(vLEDTask, "LED", 256, NULL, 3, NULL);

vTaskStartScheduler();

    while (1)
    {
    }

	
	
}