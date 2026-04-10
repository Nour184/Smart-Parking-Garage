#ifndef GATE_FSM_H
#define GATE_FSM_H

#include "shared_queues.h"

typedef enum {
	IDLE_CLOSED,
	IDLE_OPENED,
	OPENING,
	CLOSING,
	STOPPED_MIDWAY,
	REVERSING
}GateState_t;


//functions declaration 
void initGateStatus(void);
GateState_t getCurrentGateState(void);

GateState_t updateGateStatus(Event_t); //check whether i need an input here or not 
#endif