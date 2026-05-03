#include "gpio_driver.h"
#include "button_event.h"

extern QueueHandle_t xButtonInterruptQueue;

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

    AllLEDs_Off();

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

    GPIOB->CR |= 0x1F;

    GPIOB->AMSEL &= ~0x1F;
    GPIOB->AFSEL &= ~0x1F;
    GPIOB->PCTL  &= ~0x000FFFFF;

    GPIOB->DIR &= ~0x1F;
    GPIOB->DEN |=  0x1F;
    GPIOB->PUR |=  0x1F;

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
}

/* ================= LEDs ================= */
void GreenLED_On(void)  { GPIOF->DATA |= GREEN_LED_PIN; }
void GreenLED_Off(void) { GPIOF->DATA &= ~GREEN_LED_PIN; }
void RedLED_On(void)    { GPIOF->DATA |= RED_LED_PIN; }
void RedLED_Off(void)   { GPIOF->DATA &= ~RED_LED_PIN; }

void AllLEDs_Off(void)
{
    GPIOF->DATA &= ~(GREEN_LED_PIN | RED_LED_PIN);
}

/* ================= READ ================= */
/* Active-low buttons: pressed = 0, released = 1 */
uint8_t Read_DriverOpen(void)    { return ((GPIOF->DATA & DRIVER_OPEN_PIN)    == 0U); }
uint8_t Read_DriverClose(void)   { return ((GPIOF->DATA & DRIVER_CLOSE_PIN)   == 0U); }
uint8_t Read_SecurityOpen(void)  { return ((GPIOB->DATA & SECURITY_OPEN_PIN)  == 0U); }
uint8_t Read_SecurityClose(void) { return ((GPIOB->DATA & SECURITY_CLOSE_PIN) == 0U); }
uint8_t Read_OpenLimit(void)     { return ((GPIOB->DATA & OPEN_LIMIT_PIN)     == 0U); }
uint8_t Read_ClosedLimit(void)   { return ((GPIOB->DATA & CLOSED_LIMIT_PIN)   == 0U); }
uint8_t Read_Obstacle(void)      { return ((GPIOB->DATA & OBSTACLE_PIN)       == 0U); }

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