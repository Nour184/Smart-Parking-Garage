/**
 * @file button_driver.c
 * @brief Hardware Input Configuration and Debouncing - TEAM 1.
 * * TO DO:
 * - Configure GPIO ports/pins for all 7 buttons as Inputs.
 * - Implement the Input Task (High Priority) to poll buttons.
 * - Write software debouncing logic.
 * - Detect "tap" vs "hold" and send appropriate commands to the FreeRTOS Queue.
 */
 
 
 /*IMP note: 
  1) while handling the manual mode and one touch  mode by adding a threshold to detect which mode like half a second ,a sec or so
  Please dont send these events if you detect one touch mode when the user releases the button for the system to work correctly :
  EV_DRIVER_OPEN_RELEASED,
	EV_SECURITY_CLOSE_RELEASED...etc
	only send these events for one touch mode (these are sent in manual mode too):
	EV_DRIVER_OPEN_GATE,
	EV_SECURITY_CLOSE_GATE,..etc
	
	2) dont send these events to the queue when detecting an obstacle EV_DETECT_OBSTACLE but rather give the semaphore for the safety Task
	 and Team 3 is responsible of sending this event EV_DETECT_OBSTACLE to the queue
	 
	3)please use xQueueSendToFront()
	to send either EV_LIMIT_OPENING or EV_LIMIT_CLOSING to my queue so the FSM can transition to the IDLE state safely 
	for any other normal eevent send to back 
 */

#include "button_driver.h"

QueueHandle_t xButtonInterruptQueue = NULL;

/* ================= PORT F ================= */
static void GPIOF_Init(void)
{
    SYSCTL->RCGCGPIO |= (1U << 5);
    while ((SYSCTL->PRGPIO & (1U << 5)) == 0);

    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= (1U << 0) | (1U << 1) | (1U << 3) | (1U << 4);

    GPIOF->AMSEL &= ~((1U << 0) | (1U << 1) | (1U << 3) | (1U << 4));
    GPIOF->AFSEL &= ~((1U << 0) | (1U << 1) | (1U << 3) | (1U << 4));
    GPIOF->PCTL  &= ~0x000F0F0F;

    GPIOF->DIR &= ~((1U << 0) | (1U << 4));
    GPIOF->DIR |=  ((1U << 1) | (1U << 3));

    GPIOF->DEN |= ((1U << 0) | (1U << 1) | (1U << 3) | (1U << 4));
    GPIOF->PUR |= ((1U << 0) | (1U << 4));

    /* Interrupt BOTH edges: press + release */
    GPIOF->IS  &= ~((1U << 0) | (1U << 4));
    GPIOF->IBE |=  ((1U << 0) | (1U << 4));
    GPIOF->ICR |=  ((1U << 0) | (1U << 4));
    GPIOF->IM  |=  ((1U << 0) | (1U << 4));

    NVIC_SetPriority(GPIOF_IRQn, 5);
    NVIC_EnableIRQ(GPIOF_IRQn);
}

/* ================= PORT B ================= */
static void GPIOB_Init(void)
{
    SYSCTL->RCGCGPIO |= (1U << 1);
    while ((SYSCTL->PRGPIO & (1U << 1)) == 0);

	GPIOB->LOCK = 0x4C4F434B;
    GPIOB->CR |= 0x1F;

    GPIOB->AMSEL &= ~0x1F;
    GPIOB->AFSEL &= ~0x1F;
    GPIOB->PCTL  &= ~0x000FFFFF;

    GPIOB->DIR &= ~0x1F;
    GPIOB->DEN |=  0x1F;
    GPIOB->PUR &= ~0x1F;

    /* Interrupt BOTH edges: press + release */
    GPIOB->IS  &= ~0x1F;
    GPIOB->IBE |=  0x1F;
    GPIOB->ICR |=  0x1F;
    GPIOB->IM  |=  0x1F;

    NVIC_SetPriority(GPIOB_IRQn, 5);
    NVIC_EnableIRQ(GPIOB_IRQn);
}

/* ================= INIT ================= */
void GPIO_AllInit(void)
{
    GPIOF_Init();
    GPIOB_Init();

    xButtonInterruptQueue = xQueueCreate(20, sizeof(ButtonId_t));
    if (xButtonInterruptQueue == NULL)
	{
		while (1);
	}
}


/* ================= READ ================= */
/* Active-low buttons: pressed = 0, released = 1 */
uint8_t Read_DriverOpen(void)    { return ((GPIOF->DATA & DRIVER_OPEN_PIN)    == 0U); }
uint8_t Read_DriverClose(void)   { return ((GPIOF->DATA & DRIVER_CLOSE_PIN)   == 0U); }
/* Active-high buttons: pressed = 1, released = 0 */
uint8_t Read_SecurityOpen(void)  { return ((GPIOB->DATA & SECURITY_OPEN_PIN)  != 0U); }
uint8_t Read_SecurityClose(void) { return ((GPIOB->DATA & SECURITY_CLOSE_PIN) != 0U); }
uint8_t Read_OpenLimit(void)     { return ((GPIOB->DATA & OPEN_LIMIT_PIN)     != 0U); }
uint8_t Read_ClosedLimit(void)   { return ((GPIOB->DATA & CLOSED_LIMIT_PIN)   != 0U); }
uint8_t Read_Obstacle(void)      { return ((GPIOB->DATA & OBSTACLE_PIN)       != 0U); }

/* ================= ISR ================= */

void GPIOF_Handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    ButtonId_t buttonId;
    uint32_t status = GPIOF->MIS;

    GPIOF->ICR = status;

    if (status & DRIVER_OPEN_PIN)
    {
        buttonId = BTN_DRIVER_OPEN;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    if (status & DRIVER_CLOSE_PIN)
    {
        buttonId = BTN_DRIVER_CLOSE;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void GPIOB_Handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    ButtonId_t buttonId;
    uint32_t status = GPIOB->MIS;

    GPIOB->ICR = status;

    if (status & SECURITY_OPEN_PIN)
    {
        buttonId = BTN_SECURITY_OPEN;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    if (status & SECURITY_CLOSE_PIN)
    {
        buttonId = BTN_SECURITY_CLOSE;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    if (status & OPEN_LIMIT_PIN)
    {
        buttonId = BTN_OPEN_LIMIT;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    if (status & CLOSED_LIMIT_PIN)
    {
        buttonId = BTN_CLOSED_LIMIT;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    if (status & OBSTACLE_PIN)
    {
        buttonId = BTN_OBSTACLE;
        xQueueSendFromISR(xButtonInterruptQueue,
                          &buttonId,
                          &xHigherPriorityTaskWoken);
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}