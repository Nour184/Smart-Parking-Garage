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
 #include "safety_monitor.h"
 #include "led_task.h"
 #include "test_config.h"
 #include "input_task.h"
 #include "button_driver.h"
 
 #if TEST_SAFETY
 #include "shared_queues.h"
 #endif
 
void uartInputTask(void *pvParameters){
	uint8_t rxByte;
   	Event_t incomingEvent;
   	while(1) {
    	// 1. Read byte from the Python HIL script
        rxByte = UART0_ReceiveChar(); 
        incomingEvent = (Event_t)rxByte;
				#if TEST_FSM
        // 2. Route the event based on your Team 1 specifications
        if (incomingEvent == EV_LIMIT_OPENING || incomingEvent == EV_LIMIT_CLOSING) {
            // High priority: Push to the FRONT of the queue so the FSM stops immediately
            xQueueSendToFront(evQueue, &incomingEvent, portMAX_DELAY);
        } 
        else if (incomingEvent == EV_DETECT_OBSTACLE) {
            // NOTE: You mentioned Team 3 handles this via semaphore in the final build.
            // But for your Python HIL testing right now, if you want to test the REVERSING 
            // state logic in your FSM without Team 3's task, you can pass it to the front here.
            // If you don't want to test it via UART, you can just ignore this case.
            xQueueSendToFront(evQueue, &incomingEvent, portMAX_DELAY); 
        }
        else {
            // Normal priority (buttons, releases, conflicts): Push to the BACK of the queue
            xQueueSendToBack(evQueue, &incomingEvent, portMAX_DELAY); 
        }
				#endif
				#if TEST_SAFETY
				if(incomingEvent == 0x0){
					xSemaphoreGive(obstacleSemaphore);
				}
				#endif
				vTaskDelay(pdMS_TO_TICKS(10));
    }
 }
 
 int main(){
	 
	 //for testing
	 UART0_Init(); 
	 #if TEST_SAFETY
	 UART0_SendChar('s');
	 #endif
	 
	 portF_led_init();
   GPIO_AllInit();
   InputTask_Init();
	 
	 //Task Creation
	 /*
	 priorites:
	  Medium  -> 2
	  Hight   -> 3
	  Highest -> 4
	 */
	 //how much stack do i allocate??
	 xTaskCreate(gateControlTask, "gate controller task",150, NULL,2,NULL);	 
	 xTaskCreate(uartInputTask,"input task moker",300,NULL,3,NULL);
   xTaskCreate(safetyTask,"safety task for obstacle handling", 150, NULL,4,NULL);
	 xTaskCreate(LedTask, "Controls System Leds", 150, NULL, 2, NULL);
	 //call system init functions before scheduler
	 int_IPComm();
	 
	 
	 vTaskStartScheduler();
	 while(1);
 }
