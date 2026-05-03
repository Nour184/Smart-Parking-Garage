#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "input_task.h"
#include "gpio_driver.h"
#include "event.h"
#include "button_event.h"

#define DEBOUNCE_MS             20
#define ONE_TOUCH_THRESHOLD_MS  300

extern QueueHandle_t xGateEventQueue;
extern QueueHandle_t xLEDEventQueue;
extern QueueHandle_t xButtonInterruptQueue;

typedef struct
{
    uint8_t lastRaw;
    uint8_t stableState;
    TickType_t lastChangeTime;
    TickType_t pressStartTime;
    uint8_t manualSent;
} ButtonState_t;

typedef struct
{
    ButtonState_t state;
    uint8_t (*readFunc)(void);

    EventType_t pressEvent;
    EventType_t releaseEvent;
    EventType_t autoEvent;

    uint8_t supportManualAuto;
} ButtonObject_t;

static ButtonObject_t driverOpenBtn;
static ButtonObject_t driverCloseBtn;
static ButtonObject_t securityOpenBtn;
static ButtonObject_t securityCloseBtn;
static ButtonObject_t openLimitBtn;
static ButtonObject_t closedLimitBtn;
static ButtonObject_t obstacleBtn;

static void SendEvent(EventType_t type)
{
    GateEvent_t ev;

    if (type == EVT_NONE)
    {
        return;
    }

    ev.type = type;
    ev.time = xTaskGetTickCount();

    xQueueSend(xGateEventQueue, &ev, 0);
    xQueueSend(xLEDEventQueue,  &ev, 0);
}

static void Button_Init(ButtonObject_t *btn,
                        uint8_t (*readFunc)(void),
                        EventType_t pressEvent,
                        EventType_t releaseEvent,
                        EventType_t autoEvent,
                        uint8_t supportManualAuto)
{
    btn->state.lastRaw = 0;
    btn->state.stableState = 0;
    btn->state.lastChangeTime = 0;
    btn->state.pressStartTime = 0;
    btn->state.manualSent = 0;

    btn->readFunc = readFunc;
    btn->pressEvent = pressEvent;
    btn->releaseEvent = releaseEvent;
    btn->autoEvent = autoEvent;
    btn->supportManualAuto = supportManualAuto;
}

void InputTask_Init(void)
{
    Button_Init(&driverOpenBtn,
                Read_DriverOpen,
                EVT_DRIVER_OPEN_PRESS,
                EVT_DRIVER_OPEN_RELEASE,
                EVT_DRIVER_OPEN_AUTO,
                1);

    Button_Init(&driverCloseBtn,
                Read_DriverClose,
                EVT_DRIVER_CLOSE_PRESS,
                EVT_DRIVER_CLOSE_RELEASE,
                EVT_DRIVER_CLOSE_AUTO,
                1);

    Button_Init(&securityOpenBtn,
                Read_SecurityOpen,
                EVT_SECURITY_OPEN_PRESS,
                EVT_SECURITY_OPEN_RELEASE,
                EVT_SECURITY_OPEN_AUTO,
                1);

    Button_Init(&securityCloseBtn,
                Read_SecurityClose,
                EVT_SECURITY_CLOSE_PRESS,
                EVT_SECURITY_CLOSE_RELEASE,
                EVT_SECURITY_CLOSE_AUTO,
                1);

    Button_Init(&openLimitBtn,
                Read_OpenLimit,
                EVT_OPEN_LIMIT_PRESS,
                EVT_NONE,
                EVT_NONE,
                0);

    Button_Init(&closedLimitBtn,
                Read_ClosedLimit,
                EVT_CLOSED_LIMIT_PRESS,
                EVT_NONE,
                EVT_NONE,
                0);

    Button_Init(&obstacleBtn,
                Read_Obstacle,
                EVT_OBSTACLE_PRESS,
                EVT_NONE,
                EVT_NONE,
                0);
}

static void ProcessButton(ButtonObject_t *btn)
{
    TickType_t now = xTaskGetTickCount();
    uint8_t raw = btn->readFunc();

    if (raw != btn->state.lastRaw)
    {
        btn->state.lastChangeTime = now;
    }

    if ((now - btn->state.lastChangeTime) >= pdMS_TO_TICKS(DEBOUNCE_MS))
    {
        if (raw != btn->state.stableState)
        {
            btn->state.stableState = raw;

            if (btn->state.stableState == 1)
            {
                btn->state.pressStartTime = now;
                btn->state.manualSent = 0;

                if (btn->supportManualAuto == 0)
                {
                    SendEvent(btn->pressEvent);
                }
            }
            else
            {
                if (btn->supportManualAuto == 1)
                {
                    TickType_t duration = now - btn->state.pressStartTime;

                    if (btn->state.manualSent == 1)
                    {
                        SendEvent(btn->releaseEvent);
                    }
                    else
                    {
                        if (duration < pdMS_TO_TICKS(ONE_TOUCH_THRESHOLD_MS))
                        {
                            SendEvent(btn->autoEvent);
                        }
                        else
                        {
                            SendEvent(btn->releaseEvent);
                        }
                    }
                }
            }
        }

        if (btn->supportManualAuto == 1 &&
            btn->state.stableState == 1 &&
            btn->state.manualSent == 0)
        {
            TickType_t heldTime = now - btn->state.pressStartTime;

            if (heldTime >= pdMS_TO_TICKS(ONE_TOUCH_THRESHOLD_MS))
            {
                btn->state.manualSent = 1;
                SendEvent(btn->pressEvent);
            }
        }
    }

    btn->state.lastRaw = raw;
}

static void ProcessButtonById(ButtonId_t buttonId)
{
    switch (buttonId)
    {
        case BTN_DRIVER_OPEN:
            ProcessButton(&driverOpenBtn);
            break;

        case BTN_DRIVER_CLOSE:
            ProcessButton(&driverCloseBtn);
            break;

        case BTN_SECURITY_OPEN:
            ProcessButton(&securityOpenBtn);
            break;

        case BTN_SECURITY_CLOSE:
            ProcessButton(&securityCloseBtn);
            break;

        case BTN_OPEN_LIMIT:
            ProcessButton(&openLimitBtn);
            break;

        case BTN_CLOSED_LIMIT:
            ProcessButton(&closedLimitBtn);
            break;

        case BTN_OBSTACLE:
            ProcessButton(&obstacleBtn);
            break;

        default:
            break;
    }
}

static uint8_t IsManualButtonReleased(ButtonId_t buttonId)
{
    switch (buttonId)
    {
        case BTN_DRIVER_OPEN:
            return (Read_DriverOpen() == 0);

        case BTN_DRIVER_CLOSE:
            return (Read_DriverClose() == 0);

        case BTN_SECURITY_OPEN:
            return (Read_SecurityOpen() == 0);

        case BTN_SECURITY_CLOSE:
            return (Read_SecurityClose() == 0);

        default:
            return 1;
    }
}

void vInputTask(void *pvParameters)
{
    ButtonId_t buttonId;

    (void) pvParameters;

    while (1)
    {
        if (xQueueReceive(xButtonInterruptQueue,
                          &buttonId,
                          portMAX_DELAY) == pdPASS)
        {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));

            ProcessButtonById(buttonId);

            while (IsManualButtonReleased(buttonId) == 0)
            {
                ProcessButtonById(buttonId);
                vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
            }

            ProcessButtonById(buttonId);
        }
    }
}