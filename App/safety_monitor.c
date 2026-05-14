/**
 * @file safety_monitor.c
 * @brief Safety and Obstacle Detection Task (Highest Priority) - TEAM 3.
 * * TO DO:
 * - Implement a FreeRTOS task that waits (blocks) on the Obstacle Semaphore.
 * - When semaphore is given, instantly override current FSM state.
 * - Force the 0.5-second reverse logic (REVERSING state) and then stop completely.
 */
#include "gate_fsm.h"
#include "safety_monitor.h"
#include "test_config.h"

#if TEST_SAFETY
#include "uart_driver.h"
#endif

void safetyTask(void *pvParameters)
{
    while(1)
    {   
        xSemaphoreTake(obstacleSemaphore, portMAX_DELAY); // Block until I recieve an obstacle

    	GateState_t currentState = getCurrentGateState();
		if (currentState != CLOSING) continue;

        forceGateState(REVERSING); // FORCE sate to be reversing; so any input gets ignored

				xQueueReset(ledQueue); // Flush the queue
				const Led_t ev_led = EV_SET_GREEN;
				xQueueSend(ledQueue, (void *)(&ev_led), 0); // Send ev_led to LED Task, then block.
			
				#if TEST_SAFETY
				const char* msg = "Reverse started\n";
				UART0_send_string(msg);
				#endif
        vTaskDelay(pdMS_TO_TICKS(REVERSE_DELAY_MS)); // reverse for a certain period.
        forceGateState(STOPPED_MIDWAY); // change state back so inputs are not ignored by FSM.
			
				const Led_t ev_reset_led = EV_RESET_LED;
				xQueueSend(ledQueue, (void *)(&ev_reset_led), 0); // send ev_reset_led to LED Task, then block on obstacle semphr
        xQueueReset(evQueue);
				#if TEST_SAFETY
				msg = "Reverse ended\n";
				UART0_send_string(msg);
				#endif
    }
}
