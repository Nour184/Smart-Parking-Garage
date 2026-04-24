/**
 * @file safety_monitor.c
 * @brief Safety and Obstacle Detection Task (Highest Priority) - TEAM 3.
 * * TO DO:
 * - Implement a FreeRTOS task that waits (blocks) on the Obstacle Semaphore.
 * - When semaphore is given, instantly override current FSM state.
 * - Force the 0.5-second reverse logic (REVERSING state) and then stop completely.
 */
#include "shared_queues.h"
#include "gate_fsm.h"
#include "safety_monitor.h"

void safetyTask(void *pvParameters)
{
    while(1)
    {   
        xSemaphoreTake(obstacleSemaphore, portMAX_DELAY);
        forceGateState(REVERSING);
        // TODO: CHANGE LED COLOR
        vTaskDelay(pdMS_TO_TICKS(REVERSE_DELAY_MS));
        forceGateState(STOPPED_MIDWAY);
        xQueueReset(evQueue);
    }
}
