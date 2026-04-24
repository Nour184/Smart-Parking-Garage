#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "gpio_driver.h"
#include "event.h"
#include "Receive_task.h"

extern QueueHandle_t xGateEventQueue;

void vTestReceiverTask(void *pvParameters)
{
    GateEvent_t ev;

    while (1)
    {
        if (xQueueReceive(xGateEventQueue, &ev, portMAX_DELAY) == pdPASS)
        {
            AllLEDs_Off();

            switch (ev.type)
            {
                case EVT_DRIVER_OPEN_PRESS:
                    GreenLED_On();
                    break;

                case EVT_DRIVER_CLOSE_PRESS:
                    RedLED_On();
                    break;

                case EVT_SECURITY_OPEN_PRESS:
                    GreenLED_On();
                    RedLED_On();
                    break;

                case EVT_SECURITY_CLOSE_PRESS:
                    RedLED_On();
                    vTaskDelay(pdMS_TO_TICKS(100));
                    RedLED_Off();
                    break;

                case EVT_OPEN_LIMIT_PRESS:
                    GreenLED_On();
                    vTaskDelay(pdMS_TO_TICKS(100));
                    GreenLED_Off();
                    break;

                case EVT_CLOSED_LIMIT_PRESS:
                    RedLED_On();
                    GreenLED_On();
                    vTaskDelay(pdMS_TO_TICKS(100));
                    AllLEDs_Off();
                    break;

                case EVT_OBSTACLE_PRESS:
                    GreenLED_On();
                    RedLED_On();
                    break;

                default:
                    AllLEDs_Off();
                    break;
            }
        }
    }
}