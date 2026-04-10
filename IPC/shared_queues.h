#ifndef SHARED_QUEUES_H
#define SHARED_QUEUES_H
 
 #include "FreeRTOS.h"
 #include "queue.h"
 #include "semphr.h"
 
 
/*
 Events enum -added here as its shared
 NOTE: DO NOT ADD THESE EVENTS: open gate button released, close gate button realased ...etc TEAM 1 SHOULD handle this logic  
*/ 
typedef enum {
	//do we add a start event to start the system ??
	EV_DRIVER_OPEN_GATE,
	EV_DRIVER_CLOSE_GATE,
	EV_SECURITY_OPEN_GATE,
	EV_SECURITY_CLOSE_GATE,
	
	EV_LIMIT_OPENING,
	EV_LIMIT_CLOSING,
	
	EV_STOP_GATE,  //this event is sent when buttons get released (for manual mode handling)!!
	
	EV_DETECT_OBSTACLE
}Event_t;

//init mutual queues..etc
 void int_IPComm(void);

#endif