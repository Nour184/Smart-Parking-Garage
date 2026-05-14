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

typedef enum {
	NONE,
	DRIVER,
	SECURITY
}CmdOwner_t;

//functions declaration 
void initGateStatus(void);
GateState_t getCurrentGateState(void);
CmdOwner_t getCurrentOwner(void);
void forceGateState(GateState_t);
uint8_t attemptSafetyReverse(void); // used by the safety task to force the state directly.
GateState_t updateGateStatus(Event_t);
#endif
