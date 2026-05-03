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

    (void) pvParameters;

    while (1)
    {
        if (xQueueReceive(xGateEventQueue, &ev, portMAX_DELAY) == pdPASS)
        {
            switch (ev.type)
            {
                /* ================= DRIVER OPEN ================= */

                case EVT_DRIVER_OPEN_PRESS:
                    AllLEDs_Off();
                    GreenLED_On();          /* Manual opening */
                    break;

                case EVT_DRIVER_OPEN_RELEASE:
                    AllLEDs_Off();          /* Stop manual opening */
                    break;

                case EVT_DRIVER_OPEN_AUTO:
                    AllLEDs_Off();
                    GreenLED_On();          /* Auto opening */
                    break;

                /* ================= DRIVER CLOSE ================= */

                case EVT_DRIVER_CLOSE_PRESS:
                    AllLEDs_Off();
                    RedLED_On();            /* Manual closing */
                    break;

                case EVT_DRIVER_CLOSE_RELEASE:
                    AllLEDs_Off();          /* Stop manual closing */
                    break;

                case EVT_DRIVER_CLOSE_AUTO:
                    AllLEDs_Off();
                    RedLED_On();            /* Auto closing */
                    break;

                /* ================= SECURITY OPEN ================= */

                case EVT_SECURITY_OPEN_PRESS:
                    AllLEDs_Off();
                    GreenLED_On();          /* Manual opening from security */
                    break;

                case EVT_SECURITY_OPEN_RELEASE:
                    AllLEDs_Off();
                    break;

                case EVT_SECURITY_OPEN_AUTO:
                    AllLEDs_Off();
                    GreenLED_On();
                    break;

                /* ================= SECURITY CLOSE ================= */

                case EVT_SECURITY_CLOSE_PRESS:
                    AllLEDs_Off();
                    RedLED_On();            /* Manual closing from security */
                    break;

                case EVT_SECURITY_CLOSE_RELEASE:
                    AllLEDs_Off();
                    break;

                case EVT_SECURITY_CLOSE_AUTO:
                    AllLEDs_Off();
                    RedLED_On();
                    break;

                /* ================= LIMITS ================= */

                case EVT_OPEN_LIMIT_PRESS:
                    AllLEDs_Off();          /* Gate reached fully open */
                    GreenLED_On();
                    vTaskDelay(pdMS_TO_TICKS(100));
                    GreenLED_Off();
                    break;

                case EVT_CLOSED_LIMIT_PRESS:
                    AllLEDs_Off();          /* Gate reached fully closed */
                    RedLED_On();
                    vTaskDelay(pdMS_TO_TICKS(100));
                    RedLED_Off();
                    break;

                /* ================= OBSTACLE ================= */

                case EVT_OBSTACLE_PRESS:
                    AllLEDs_Off();
                    GreenLED_On();
                    RedLED_On();            /* Obstacle indication */
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