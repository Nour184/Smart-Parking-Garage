#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "gpio_driver.h"
#include "event.h"

extern QueueHandle_t xLEDEventQueue;

void vLEDTask(void *pvParameters)
{
    GateEvent_t ev;

    (void) pvParameters;

    AllLEDs_Off();

    while (1)
    {
        if (xQueueReceive(xLEDEventQueue, &ev, portMAX_DELAY) == pdPASS)
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

                case EVT_DRIVER_OPEN_RELEASE:
                case EVT_SECURITY_OPEN_RELEASE:
                case EVT_DRIVER_CLOSE_RELEASE:
                case EVT_SECURITY_CLOSE_RELEASE:
                    AllLEDs_Off();
                    break;

                case EVT_OPEN_LIMIT_PRESS:
                case EVT_CLOSED_LIMIT_PRESS:
                    AllLEDs_Off();
                    break;

                case EVT_OBSTACLE_PRESS:
                    AllLEDs_Off();
                    GreenLED_On();
                    RedLED_On();
                    vTaskDelay(pdMS_TO_TICKS(200));
                    AllLEDs_Off();
                    break;

                default:
                    AllLEDs_Off();
                    break;
            }
        }
    }
}