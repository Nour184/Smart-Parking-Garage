/**
 * @file shared_queues.c
 * @brief Inter-Process Communication Definitions - ALL TEAMS.
 * * TO DO:
 * - Define the global FreeRTOS Queue handles.
 * - Define the global Mutex handles.
 * - Define the global Semaphores.
 * - initialize all of these before the scheduler starts.
 */
 
 #include "shared_queues.h"

QueueHandle_t evQueue = NULL;
SemaphoreHandle_t stateMutex = NULL;
 
 void int_IPComm(void){
	 //what size should we assign??
	 evQueue = xQueueCreate(15,sizeof(Event_t));
	 
	 stateMutex = xSemaphoreCreateMutex();
	 //safety catch if mutex or queue werent initialized correctly
	 if(stateMutex == NULL || evQueue == NULL){
		 while(1);
	 }
 }
 