#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>
#include "tm4c123gh6pm.h"

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

/* LEDs */
#define RED_LED_PIN         (1U << 1)   // PF1
#define GREEN_LED_PIN       (1U << 3)   // PF3

/* Driver Buttons */
#define DRIVER_OPEN_PIN     (1U << 4)   // PF4
#define DRIVER_CLOSE_PIN    (1U << 0)   // PF0

/* Security / Limit / Obstacle Buttons */
#define SECURITY_OPEN_PIN   (1U << 0)   // PB0
#define SECURITY_CLOSE_PIN  (1U << 1)   // PB1
#define OPEN_LIMIT_PIN      (1U << 2)   // PB2
#define CLOSED_LIMIT_PIN    (1U << 3)   // PB3
#define OBSTACLE_PIN        (1U << 4)   // PB4

void GPIO_AllInit(void);

/* LEDs */
void GreenLED_On(void);
void GreenLED_Off(void);
void RedLED_On(void);
void RedLED_Off(void);
void AllLEDs_Off(void);

/* Buttons: return 1 if PRESSED, 0 if RELEASED */
uint8_t Read_DriverOpen(void);
uint8_t Read_DriverClose(void);
uint8_t Read_SecurityOpen(void);
uint8_t Read_SecurityClose(void);
uint8_t Read_OpenLimit(void);
uint8_t Read_ClosedLimit(void);
uint8_t Read_Obstacle(void);

#endif