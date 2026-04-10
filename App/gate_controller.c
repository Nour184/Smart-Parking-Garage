/**
 * @file gate_controller.c
 * @brief Main Gate Control Task (Medium Priority) - TEAM 2.
 * * TO DO:
 * - Implement the FreeRTOS task loop for the gate.
 * - Read from the IPC Queue to get button events.
 * - Use the Mutex to lock the shared gate state.
 * - Pass the received events into the FSM logic to determine the next action.
 */
 

#include "gate_controller.h"


//implement the function that is called by xTaskCreate() in main!!

void gateControlTask(void* pvParameters){
	//init gate state 
	//initGateStatus();
	Event_t incomingEV;
	while(1){
		//continously read from queue
		BaseType_t status = xQueueReceive(evQueue,&incomingEV,portMAX_DELAY); //block if no incoming events found
		
		if(status == pdTRUE){
		//try to acquire the mutex to update the gate state
		 if(xSemaphoreTake(stateMutex, portMAX_DELAY) == pdTRUE){ //block untill i can acquire the mutex
			 //updateGateStatus(incomingEV); //update the gate status 
			 xSemaphoreGive(stateMutex);
		 }
	 }
	}
}