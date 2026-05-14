#ifndef SHARED_QUEUES_H
#define SHARED_QUEUES_H
 
 #include "FreeRTOS.h"
 #include "queue.h"
 #include "semphr.h"
 #include "basic_io.h"
 
 
typedef enum {

	EV_DRIVER_OPEN_GATE,
	EV_DRIVER_CLOSE_GATE,
	EV_SECURITY_OPEN_GATE,
	EV_SECURITY_CLOSE_GATE,
	
	EV_LIMIT_OPENING,
	EV_LIMIT_CLOSING,
	
	//release events for handling manual mode
  EV_DRIVER_OPEN_RELEASED,
	EV_DRIVER_CLOSE_RELEASED,
	EV_SECURITY_OPEN_RELEASED,
	EV_SECURITY_CLOSE_RELEASED,
	
	//conflict events
	EV_DRIVER_CONFLICT, //send this when we catch simulatneous open and close signals(AT THE SAME TIME) FROM THE DRIVER'S PANEL ONLY
	EV_SECURITY_CONFLICT,//send this when we catch simulatneous open and close signals(AT THE SAME TIME) FROM THE SECURITY'S PANEL ONLY
	
	EV_DETECT_OBSTACLE, 
	EV_REVERSE_TIMEOUT 
} Event_t;

typedef enum{
    EV_SET_GREEN,
    EV_SET_RED,
    EV_RESET_LED
}Led_t;


//queue and mutex handler
 extern QueueHandle_t evQueue; //shared queue used to communicate the incoming events (door open , door close, ....etc)
 extern SemaphoreHandle_t stateMutex;  //use this Mutex when accessing the gate state variable as its a shared resource!!
 extern SemaphoreHandle_t obstacleSemaphore; // safety tasks blocks until obstacle semaphore is signaled
 extern QueueHandle_t printQueue;
//init mutual queues..etc
 void int_IPComm(void);
 // used for sending commands to LED's
 extern QueueHandle_t ledQueue;

#endif
