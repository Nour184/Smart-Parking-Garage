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
 #include "uart_task.h"
 
 #if TEST_SAFETY
 #include "shared_queues.h"
 #endif
 
 void uartInputTask(void *pvParameters); //used for uart unit testing
 
#include "gate_controller.h"
#include "safety_monitor.h"
#include "led_task.h"
#include "test_config.h"
#include "input_task.h"
#include "button_driver.h"
#include "uart_task.h"

#if TEST_SAFETY
#include "shared_queues.h"
#endif

void uartInputTask(void *pvParameters); 

int main(){
    UART0_Init(); 
    portF_led_init();
    GPIO_AllInit();
    InputTask_Init();

    #if TEST_SAFETY
    UART0_SendChar('s');
    #endif

    // Global Inter-Process Communication (Queues/Mutexes) Initialization
    // Guarantees handles are valid before any Task Control Block references them.
    int_IPComm();

    // Safe compound literal cast pointing to static Flash memory
    xQueueSend(printQueue, &(char*){"Gate Ctrl System Started...\r\n"}, 0);

    /*
     Task Priorities:
      Low     -> 1 (UART output stream)
      Medium  -> 2 (Gate & LEDs)
      High    -> 3 (Input sampling)
      Highest -> 4 (Safety critical overrides)
    */
    
    xTaskCreate(gateControlTask, "Gate Controller", 150, NULL, 2, NULL);
    xTaskCreate(vInputTask,      "Input Driver",    256, NULL, 3, NULL);
    xTaskCreate(safetyTask,      "Safety Monitor",  150, NULL, 4, NULL);
    xTaskCreate(LedTask,         "System LEDs",     150, NULL, 2, NULL);
    xTaskCreate(uartTask,        "UART TX Engine",  150, NULL, 1, NULL);
    
    // xTaskCreate(uartInputTask, "Input Mocker",   300, NULL, 3, NULL);

    vTaskStartScheduler();
    
    // Execution should never reach this trap unless heap allocation fails during boot
    while(1);
}

// ... uartInputTask remains unchanged ...
 
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
