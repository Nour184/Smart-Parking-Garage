#ifndef SHARED_QUEUES_H
#define SHARED_QUEUES_H
 
 #include "FreeRTOS.h"
 #include "queue.h"
 #include "semphr.h"
 
 
typedef enum {
	//do we add a start event to start the system ??
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
	EV_DRIVER_CONFLICT, //TEAM 1 sends this when they catch simulatneous open and close signals(AT THE SAME TIME) FROM THE DRIVER'S PANEL ONLY
	EV_SECURITY_CONFLICT,//TEAM 1 sends this when they catch simulatneous open and close signals(AT THE SAME TIME) FROM THE SECURITY'S PANEL ONLY
	
	EV_DETECT_OBSTACLE
}Event_t;


//queue and mutex handler
 extern QueueHandle_t evQueue;
 extern SemaphoreHandle_t stateMutex; 

//init mutual queues..etc
 void int_IPComm(void);

#endif