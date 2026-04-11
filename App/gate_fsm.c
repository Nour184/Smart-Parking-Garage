#include "gate_fsm.h"
 
 //global current state and ownership var
 static GateState_t currentGateState;
 static CmdOwner_t currentOwner = NONE;
 
 
 void initGateStatus(void){
	 //set init state var to IDLE CLOSED 
	 currentGateState = IDLE_CLOSED;
	 currentOwner = NONE;
 }
 
 GateState_t getCurrentGateState(void){  //must acquire the mutex first!!
	 GateState_t gateState;
	 if(xSemaphoreTake(stateMutex,portMAX_DELAY)){ //block until you actually take it
		 gateState = currentGateState;
		 xSemaphoreGive(stateMutex);
	 }
	 return gateState; //since we cant return from the function without returning the mutex first (deadlock)!!
 }
 
 //some function handlers
 static void handle_IdleClosed(Event_t ev){
	 if(ev == EV_SECURITY_OPEN_GATE){ //higher priority
		 currentGateState = OPENING;
		 currentOwner = SECURITY;
	 }
	 else if(ev == EV_DRIVER_OPEN_GATE){
		 currentGateState = OPENING;
		 currentOwner = DRIVER;
	 }
 }
 
 static void handle_IdleOpened(Event_t ev){
	 if(ev == EV_SECURITY_CLOSE_GATE){
		 currentGateState = CLOSING;
		 currentOwner = SECURITY;
	 }
	 else if(ev == EV_DRIVER_CLOSE_GATE){
		 currentGateState = CLOSING;
		 currentOwner = DRIVER;
	 }
 }
 
 static void handle_Opening(Event_t ev){
	 switch(ev){
		 case EV_LIMIT_OPENING: //handling limit reached case
			 currentGateState = IDLE_OPENED;
		   currentOwner = NONE; //reset ownership
		  break;
		 case EV_SECURITY_CONFLICT:
			 currentGateState = STOPPED_MIDWAY; //no need to check the currentOwner here as security has the highest priority always
		   currentOwner = NONE;
		  break;
		 case EV_DRIVER_CONFLICT:
			 if(currentOwner == DRIVER){    //if owner is SECURITY ignore the conflict!!
			 currentGateState = STOPPED_MIDWAY;
			 currentOwner = NONE;
		 }
			 break;
		 case EV_SECURITY_CLOSE_GATE:
			 currentGateState = CLOSING;
		   currentOwner = SECURITY;
		  break;
		 case EV_SECURITY_OPEN_GATE: //upgrade ownership if the currentOwner is not security!!
			 currentOwner = SECURITY;
		  break;
		 case EV_DRIVER_CLOSE_GATE:
			 if(currentOwner == DRIVER){
			 currentGateState = CLOSING;
			 currentOwner = DRIVER;
		 }
		 break;
		 //handle manual mode - releasing the button
		 case EV_SECURITY_OPEN_RELEASED:
			 if(currentOwner == SECURITY) {
       currentGateState = STOPPED_MIDWAY;
       currentOwner = NONE;
     }
			 break;
		 case EV_DRIVER_OPEN_RELEASED:
			 if(currentOwner == DRIVER) {
       currentGateState = STOPPED_MIDWAY;
       currentOwner = NONE;
      }
			 break;
		 default :
			 break;
	 }
 }
 
 static void handle_Closing(Event_t ev){
	 switch(ev){
		 case EV_DETECT_OBSTACLE: //highest priority of all 
			 currentGateState = REVERSING;
		   currentOwner = NONE;  //reset onwnership
		   xQueueReset(evQueue); //SOOOOO IMP TO FLUSH THE QUEUE FROM OLDER EVENTS THAT WE NO LONGER NEED TO EXECUTE!!!
		  break;
		 case EV_LIMIT_CLOSING:
			 currentGateState = IDLE_CLOSED;
		   currentOwner = NONE;
		  break; 
		 //handle conflicts
		 case EV_SECURITY_CONFLICT: //no need to check ownership
			 currentGateState = STOPPED_MIDWAY;
		   currentOwner = NONE;
		  break;
		 case EV_DRIVER_CONFLICT:
			 if(currentOwner == DRIVER){
				 currentGateState = STOPPED_MIDWAY;
				 currentOwner = NONE;
			 }
		  break;
			 //handle security commands
		 case EV_SECURITY_OPEN_GATE: //one touch case by the way so no conflict
			 currentGateState = OPENING;
		   currentOwner = SECURITY;
		  break;
		 case EV_SECURITY_CLOSE_GATE: //change ownership only!!
			 currentOwner = SECURITY;
		  break;
		 
		 //handle driver commands
		 case EV_DRIVER_OPEN_GATE:
			 if(currentOwner == DRIVER){
				 currentGateState = OPENING;
				 currentOwner = DRIVER;
			 }
		  break;
			 //handle release event for manual mode
		 case EV_SECURITY_CLOSE_RELEASED:
			 if(currentOwner == SECURITY){
				 currentGateState = STOPPED_MIDWAY;
				 currentOwner = NONE;
			 }
			 break;
		 case EV_DRIVER_CLOSE_RELEASED:
			 if(currentOwner == DRIVER){
				 currentGateState = STOPPED_MIDWAY;
				 currentOwner = NONE;
			 }
			 break;
			
		default: 
			break;
	 }
	 
 }
 static void handle_StoppingMidway(Event_t ev){
	 switch(ev){
		 case EV_SECURITY_CONFLICT:
		 case EV_DRIVER_CONFLICT:
			 currentGateState = STOPPED_MIDWAY; //for the test case ig TC-15/16 
		   currentOwner = NONE;
		  break; 
		 //security cmds come first
		 case EV_SECURITY_OPEN_GATE:
			 currentGateState = OPENING;
		   currentOwner = SECURITY;
		 break;
		 case EV_SECURITY_CLOSE_GATE:
			 currentGateState = CLOSING;
		   currentOwner = SECURITY;
		 break;
		 //lastly drivers cmds
		 case EV_DRIVER_OPEN_GATE:
			 currentGateState = OPENING;
		   currentOwner = DRIVER;
		  break;
		 case EV_DRIVER_CLOSE_GATE:
			 currentGateState = CLOSING;
		   currentOwner = DRIVER;
		  break;
		 
		 default:
			 break;
	 }
 }
 static void handle_Reversing(Event_t ev){
	 switch(ev){
		 case EV_REVERSE_TIMEOUT:
			 currentGateState = STOPPED_MIDWAY;
		   currentOwner = NONE;
		 break;
		 default:
			 break;
	 }
 }
	 
//logic for updating the gate state based on the input events form the queue
 GateState_t updateGateStatus(Event_t ev){
	 switch(currentGateState){
		 case IDLE_CLOSED:
			 handle_IdleClosed(ev);
			 break;
		 case IDLE_OPENED:
			 handle_IdleOpened(ev);
		 break;
		 case OPENING:
			 handle_Opening(ev);
		 break;
		 case CLOSING:
			 handle_Closing(ev);
		 break;
		 case STOPPED_MIDWAY: 
			 handle_StoppingMidway(ev);
		 break;
		 case REVERSING: //handle reversing logic here 
			 //DO WHAT U GOTTA DO YA WALADY BRDO - Handled by TEAM 3!!   la dh tl3 ana hehe :) zorry
		   handle_Reversing(ev);
		 break;
	 }
	 return currentGateState;
 }
 