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
	
	EV_DETECT_OBSTACLE, //TEAM 3 also sends it before executing the safety task (right after entering it) to display the state coreclty and also to handle the gate state transition right
	EV_REVERSE_TIMEOUT  //TEAM 3 will send it after the timer(0.5 sec) is done 
}Event_t;


//queue and mutex handler
 extern QueueHandle_t evQueue; //shared queue used to communicate the incoming events (door open , door close, ....etc)
 extern SemaphoreHandle_t stateMutex;  //use this Mutex when accessing the gate state variable as its a shared resource!!

//init mutual queues..etc
 void int_IPComm(void);

#endif