#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "gpio_driver.h"
#include "event.h"

extern QueueHandle_t xGateEventQueue;

void vLEDTask(void *pvParameters)
{
    GateEvent_t ev;

    while (1)
    {
        if (xQueueReceive(xGateEventQueue, &ev, portMAX_DELAY) == pdPASS)
        {
            switch (ev.type)
            {
                case EVT_DRIVER_OPEN_PRESS:
                case EVT_SECURITY_OPEN_PRESS:
                case EVT_DRIVER_OPEN_AUTO:
                case EVT_SECURITY_OPEN_AUTO:

                    GreenLED_On();
                    RedLED_Off();
                    break;

                case EVT_DRIVER_CLOSE_PRESS:
                case EVT_SECURITY_CLOSE_PRESS:
                case EVT_DRIVER_CLOSE_AUTO:
                case EVT_SECURITY_CLOSE_AUTO:

                    RedLED_On();
                    GreenLED_Off();
                    break;

                default:
                    break;
            }
        }
    }
}