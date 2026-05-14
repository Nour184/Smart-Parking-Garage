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
	//init gate state and ownership
	initGateStatus();
	Event_t incomingEV;
	while(1){
		//continously read from queue
		BaseType_t status = xQueueReceive(evQueue,&incomingEV,portMAX_DELAY); //block if no incoming events found

		if(status == pdTRUE){
		//try to acquire the mutex to update the gate state
			vPrintString("[GATE TASK] Woke up! Processing Event: ");
			vPrintString((char*)EventStrings[incomingEV]);
			vPrintString("\r\n");
			
			if(xSemaphoreTake(stateMutex, portMAX_DELAY) == pdTRUE){ //block untill i can acquire the mutex
			 vPrintString("[GATE TASK] Acquired the Gate State Mutex. \n");
			 CmdOwner_t ownerBefore = getCurrentOwner(); //for testing script
			 
			 GateState_t state = updateGateStatus(incomingEV); //update the gate status 
			 
			 CmdOwner_t ownerAfter = getCurrentOwner(); //for testing script
				
			 vPrintString("[GATE TASK] FSM Updated. New State: ");
			 vPrintString((char*)GateStateStrings[state]);
			 vPrintString("\r\n");
			 vPrintString("[GATE TASK] Releasing the Gate State Mutex. \n");
			 xSemaphoreGive(stateMutex);
			 //for testing script
			 #if TEST_FSM
			 UART0_SendChar((uint8_t)ownerBefore); 
			 UART0_SendChar((uint8_t)state);
			 UART0_SendChar((uint8_t)ownerAfter);
			 #endif
		 }
	 }
	}
}