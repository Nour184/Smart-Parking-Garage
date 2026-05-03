#ifndef BUTTON_DRIVER_H
#define BUTTON_DRIVER_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

#include "FreeRTOS.h"
#include "queue.h"

/* ========= Pin Mapping =========
   Port F:
   PF1 -> Red LED
   PF3 -> Green LED
   PF4 -> Driver OPEN button
   PF0 -> Driver CLOSE button

   Port B:
   PB0 -> Security OPEN
   PB1 -> Security CLOSE
   PB2 -> Open Limit
   PB3 -> Closed Limit
   PB4 -> Obstacle
*/

/* Driver Buttons */
#define DRIVER_OPEN_PIN     (1U << 4)
#define DRIVER_CLOSE_PIN    (1U << 0)

/* Other Buttons */
#define SECURITY_OPEN_PIN   (1U << 0)
#define SECURITY_CLOSE_PIN  (1U << 1)
#define OPEN_LIMIT_PIN      (1U << 2)
#define CLOSED_LIMIT_PIN    (1U << 3)
#define OBSTACLE_PIN        (1U << 4)

/* Button Event */
typedef enum
{
    BTN_DRIVER_OPEN = 0,
    BTN_DRIVER_CLOSE,

    BTN_SECURITY_OPEN,
    BTN_SECURITY_CLOSE,

    BTN_OPEN_LIMIT,
    BTN_CLOSED_LIMIT,
    BTN_OBSTACLE

 } ButtonId_t;

/* Init */
void GPIO_AllInit(void);

/* Read Buttons */
uint8_t Read_DriverOpen(void);
uint8_t Read_DriverClose(void);
uint8_t Read_SecurityOpen(void);
uint8_t Read_SecurityClose(void);
uint8_t Read_OpenLimit(void);
uint8_t Read_ClosedLimit(void);
uint8_t Read_Obstacle(void);

#endif