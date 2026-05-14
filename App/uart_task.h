#ifndef UART_TASK_H
#define UART_TASK_H

#include "uart_driver.h"
#include "shared_queues.h"

void uartTask(void *pvParameters);
#endif