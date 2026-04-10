#ifndef GATE_CONTROLLER_H
#define GATE_CONTROLLER_H

#include "shared_queues.h"
#include "gate_fsm.h"
#include "FreeRTOS.h"
#include "semphr.h"

void gateControlTask(void* pvParameters);



#endif