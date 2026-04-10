/**
 * @file state_machine.c
 * @brief Finite State Machine Logic - TEAM 2.
 * * TO DO:
 * - Define the state variables (IDLE_OPEN, CLOSING, etc.).
 * - Implement the switch-case logic that transitions states based on queue inputs. 
 * Keep this purely logical and return the next state to gate_controller.c.
 */
 #include "gate_fsm.h"
 
 //global current state var
 static GateState_t currentGateState;
 
 void initGateStatus(void){
	 //set init state var to IDLE CLOSED 
	 currentGateState = IDLE_CLOSED;
 }
 
 GateState_t getCurrentGateState(void){
	 return currentGateState;
 }
 
 GateState_t updateGateStatus(Event_t ev){ //logic for updating the gate state based on the input events form the queue
	 switch(currentGateState){
		 case IDLE_CLOSED:
			 if((ev == EV_DRIVER_OPEN_GATE) || (ev == EV_SECURITY_OPEN_GATE)){
				 currentGateState = OPENING;
			 }
		 break;
		 case IDLE_OPENED:
			 if((ev == EV_DRIVER_CLOSE_GATE) || (ev == EV_SECURITY_CLOSE_GATE)){
				 currentGateState = CLOSING;
			 }
		 break;
		 case OPENING:
			 //reached the limit
			 if(ev == EV_LIMIT_OPENING){
				 currentGateState = IDLE_OPENED;
			 }
			 //handle conflicting open + close , handle button released 
			 if((ev == EV_DRIVER_CLOSE_GATE) || (ev == EV_SECURITY_CLOSE_GATE ) || (ev == EV_STOP_GATE)){
				 currentGateState = STOPPED_MIDWAY;
			 }
		 break;
		 case CLOSING:
			 if(ev == EV_LIMIT_CLOSING){
				 currentGateState = IDLE_CLOSED;
			 }
			 //detect obstacle logic
			 if(ev == EV_DETECT_OBSTACLE){
				 //i think i should communicate with the safety monitor task using a queue or something to signal them 
         //DO WHAT U GOTTA DO YA WALADY ;)
			 }
			 //conflicting close + open , handle button released
			 if((ev == EV_DRIVER_OPEN_GATE) || (ev == EV_SECURITY_OPEN_GATE) || (ev == EV_STOP_GATE)){
				 currentGateState = STOPPED_MIDWAY;
			 }
		 break;
		 case STOPPED_MIDWAY: 
			 if((ev == EV_DRIVER_OPEN_GATE) || (ev == EV_SECURITY_OPEN_GATE)){
				 currentGateState = OPENING;
			 }
			 if((ev == EV_DRIVER_CLOSE_GATE) || (ev == EV_SECURITY_CLOSE_GATE)){
				 currentGateState = CLOSING;
			 }
		 break;
		 case REVERSING: //handle reversing logic here 
			 //DO WHAT U GOTTA DO YA WALADY BRDO
		 break;
		 
	 }
	 return currentGateState;
 }
 