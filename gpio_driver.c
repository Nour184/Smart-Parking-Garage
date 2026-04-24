#include "gpio_driver.h"

static void GPIOF_Init(void)
{
    SYSCTL->RCGCGPIO |= (1U << 5);
    while ((SYSCTL->PRGPIO & (1U << 5)) == 0);

    /* Unlock PF0 */
    GPIOF->LOCK = 0x4C4F434B;
    GPIOF->CR |= (1U << 0) | (1U << 1) | (1U << 3) | (1U << 4);

    /* Disable analog / alt functions */
    GPIOF->AMSEL &= ~((1U << 0) | (1U << 1) | (1U << 3) | (1U << 4));
    GPIOF->AFSEL &= ~((1U << 0) | (1U << 1) | (1U << 3) | (1U << 4));
    GPIOF->PCTL  &= ~0x000F0F0F;

    /* PF1, PF3 output / PF0, PF4 input */
    GPIOF->DIR &= ~((1U << 0) | (1U << 4));
    GPIOF->DIR |=  ((1U << 1) | (1U << 3));

    GPIOF->DEN |= ((1U << 0) | (1U << 1) | (1U << 3) | (1U << 4));

    /* Pull-up for buttons */
    GPIOF->PUR |= ((1U << 0) | (1U << 4));

    AllLEDs_Off();
}

static void GPIOB_Init(void)
{
    SYSCTL->RCGCGPIO |= (1U << 1);
    while ((SYSCTL->PRGPIO & (1U << 1)) == 0);

    GPIOB->CR |= 0x1F;

    GPIOB->AMSEL &= ~0x1F;
    GPIOB->AFSEL &= ~0x1F;
    GPIOB->PCTL  &= ~0x000FFFFF;

    GPIOB->DIR &= ~0x1F;     /* PB0..PB4 inputs */
    GPIOB->DEN |=  0x1F;
    GPIOB->PUR |=  0x1F;     /* active-low buttons */
	//GPIOB->DIR |= 0x1F;   // PB0..PB4 output ??????
//  GPIOB->DEN |= 0x1F;
//	GPIOB->DATA ^= 0x1F;
}

void GPIO_AllInit(void)
{
    GPIOF_Init();
    GPIOB_Init();
}

/* ========= LEDs ========= */
void GreenLED_On(void)   { GPIOF->DATA |= GREEN_LED_PIN; }
void GreenLED_Off(void)  { GPIOF->DATA &= ~GREEN_LED_PIN; }
void RedLED_On(void)     { GPIOF->DATA |= RED_LED_PIN; }
void RedLED_Off(void)    { GPIOF->DATA &= ~RED_LED_PIN; }

void AllLEDs_Off(void)
{
    GPIOF->DATA &= ~(GREEN_LED_PIN | RED_LED_PIN);
}

/* ========= Buttons ========= */
/* active-low => pressed when bit == 0 */
uint8_t Read_DriverOpen(void)    { return ((GPIOF->DATA & DRIVER_OPEN_PIN)    == 0U) ? 1U : 0U; }
uint8_t Read_DriverClose(void)   { return ((GPIOF->DATA & DRIVER_CLOSE_PIN)   == 0U) ? 1U : 0U; }
uint8_t Read_SecurityOpen(void)  { return ((GPIOB->DATA & SECURITY_OPEN_PIN)  == 0U) ? 1U : 0U; }
uint8_t Read_SecurityClose(void) { return ((GPIOB->DATA & SECURITY_CLOSE_PIN) == 0U) ? 1U : 0U; }
uint8_t Read_OpenLimit(void)     { return ((GPIOB->DATA & OPEN_LIMIT_PIN)     == 0U) ? 1U : 0U; }
uint8_t Read_ClosedLimit(void)   { return ((GPIOB->DATA & CLOSED_LIMIT_PIN)   == 0U) ? 1U : 0U; }
uint8_t Read_Obstacle(void)      { return ((GPIOB->DATA & OBSTACLE_PIN)       == 0U) ? 1U : 0U; }