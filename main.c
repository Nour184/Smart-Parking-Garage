# include "FreeRTOS.h"
# include "task.h"
# include "basic_io.h"
# include "gpio_driver.h"
# include "event.h"
# include "Input_task.h"
#include "queue.h"
# include "Receive_task.h"

QueueHandle_t xGateEventQueue;

void vTestReceiverTask(void *pvParameters);

int main(void)
{
    GPIO_AllInit();

    xGateEventQueue = xQueueCreate(20, sizeof(GateEvent_t));
     InputTask_Init();
    xTaskCreate(vInputTask, "InputTask", 256, NULL, 3, NULL);
    xTaskCreate(vTestReceiverTask, "Receiver", 256, NULL, 1, NULL);

    vTaskStartScheduler();

    while (1)
    {
    }

	
	
}