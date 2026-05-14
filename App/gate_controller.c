/**
 * @file gate_controller.c
 * @brief Main Gate Control Task (Medium Priority) 
 * * TO DO:
 * - Implement the FreeRTOS task loop for the gate.
 * - Read from the IPC Queue to get button events.
 * - Use the Mutex to lock the shared gate state.
 * - Pass the received events into the FSM logic to determine the next action.
 */
 

#include "gate_controller.h"
#include "test_config.h"
#include <stdio.h>

// String lookup table for Gate States printing
static const char* GateStateStrings[] = {
    "IDLE_CLOSED",     // Index 0
    "IDLE_OPENED",     // Index 1
    "OPENING",         // Index 2
    "CLOSING",         // Index 3
    "STOPPED_MIDWAY",  // Index 4
    "REVERSING"        // Index 5
};

static const char* EventStrings[] = {  // for printing the exact event names
    "EV_DRIVER_OPEN_GATE",
    "EV_DRIVER_CLOSE_GATE",
    "EV_SECURITY_OPEN_GATE",
    "EV_SECURITY_CLOSE_GATE",
    "EV_LIMIT_OPENING",
    "EV_LIMIT_CLOSING",
    "EV_DRIVER_OPEN_RELEASED",
    "EV_DRIVER_CLOSE_RELEASED",
    "EV_SECURITY_OPEN_RELEASED",
    "EV_SECURITY_CLOSE_RELEASED",
    "EV_DRIVER_CONFLICT",
    "EV_SECURITY_CONFLICT",
    "EV_DETECT_OBSTACLE",
    "EV_REVERSE_TIMEOUT"
};

void gateControlTask(void* pvParameters){
    // Init gate state and ownership
    initGateStatus();
    Event_t incomingEV;
    
    while(1){
        // Continuously read from queue
        BaseType_t status = xQueueReceive(evQueue, &incomingEV, portMAX_DELAY);

        if(status == pdTRUE){
            // Safely resolve the event string using your lookup table
            // Adding \r\n keeps your serial monitor formatting perfectly clean.
            //static char outputBuffer_temp[64];
            //sprintf(outputBuffer_temp, "[GATE TASK] Pulled Event: %s\r\n", EventStrings[incomingEV]);
            
            //char *strPtr = outputBuffer_temp;
            //xQueueSend(printQueue, &strPtr, 0);
            
            // Artificial Delay for TC19
            //vTaskDelay(pdMS_TO_TICKS(2000)); 
            
            // Try to acquire the mutex to update the gate state
            if(xSemaphoreTake(stateMutex, portMAX_DELAY) == pdTRUE){ 
							  //xQueueSend(printQueue, (void*)&(char*){"[GATE TASK] Mutex Acquired! Simulating heavy load...\r\n"}, 0); // for TC 20

                CmdOwner_t ownerBefore = getCurrentOwner(); 
                GateState_t state = updateGateStatus(incomingEV); 
								//for(volatile uint32_t i = 0; i < 8000000; i++) {}

                CmdOwner_t ownerAfter = getCurrentOwner(); 
                
                static char outputBuffer[64];
                sprintf(outputBuffer, "[current state]: %s\r\n", GateStateStrings[state]);
                
                // Reusing strPtr safely to point to the new buffer
                char* strPtr = outputBuffer;
                xQueueSend(printQueue, &strPtr, 0);
									
                //xQueueSend(printQueue, (void*)&(char*){"[GATE TASK] Processing done. Releasing Mutex...\r\n"}, 0); // for TC 20
                xSemaphoreGive(stateMutex);
                
                // For testing script
                #if TEST_FSM
                UART0_SendChar((uint8_t)ownerBefore); 
                UART0_SendChar((uint8_t)state);
                UART0_SendChar((uint8_t)ownerAfter);
                #endif
            }
        }
    }
}