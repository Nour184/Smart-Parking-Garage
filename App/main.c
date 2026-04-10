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
 #include "FreeRTOS.h"
 #include "task.h"
 #include "gate_controller.h"
 
 int main(){
	 
	 //Task Creation
	 //how much stack do i allocate?? and what priority do i assign-> its supposed to be medium 
	 xTaskCreate(gateControlTask, "gate controller task",300, NULL,3,NULL);	 
	 
	 //call system init functions before scheduler
	 int_IPComm();
	 
	 
	 vTaskStartScheduler();
	 while(1);
 }