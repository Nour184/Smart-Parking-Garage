#ifndef INPUT_TASK_H
#define INPUT_TASK_H

#include "FreeRTOS.h"

extern QueueHandle_t xButtonInterruptQueue;

void vInputTask(void *pvParameters);
void InputTask_Init(void);

#endif