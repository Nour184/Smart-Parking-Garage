/**
 * @file main.c
 * @brief System Initialization and RTOS Scheduler.
 * * TO DO: 
 * - Initialize system clock.
 * - Call hardware init functions from the Drivers folder.
 * - Call queue/semaphore creation functions.
 * - Create FreeRTOS tasks
 * - Start the scheduler.
 */

 #include "gate_controller.h"
 
 int main(){
	 
	 //Task Creation
	 /*
	 priorites:
	  Medium  -> 2
	  Hight   -> 3
	  Highest -> 4
	 */
	 //how much stack do i allocate??
	 xTaskCreate(gateControlTask, "gate controller task",150, NULL,2,NULL);	 
	 
	 //call system init functions before scheduler
	 int_IPComm();
	 
	 
	 vTaskStartScheduler();
	 while(1);
 }