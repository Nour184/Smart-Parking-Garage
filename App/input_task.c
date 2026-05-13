#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
# include "../IPC/shared_queues.h"
#include "input_task.h"
#include "../Drivers/button_driver.h"
#include "../Drivers/event.h"

#define DEBOUNCE_MS             20
#define ONE_TOUCH_THRESHOLD_MS  300

typedef struct
{
    TickType_t lastChangeTime;
    uint8_t isLastEventPress;
} ButtonState_t;

typedef struct
{
    ButtonState_t state;
    uint8_t (*readFunc)(void);

    EventType_t pressEvent;
    EventType_t releaseEvent;
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
    Event_t eventToSend;

    switch (type)
    {
        case EVT_DRIVER_OPEN_PRESS:
        case EVT_DRIVER_OPEN_AUTO:
            eventToSend = EV_DRIVER_OPEN_GATE;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_DRIVER_CLOSE_PRESS:
        case EVT_DRIVER_CLOSE_AUTO:
            eventToSend = EV_DRIVER_CLOSE_GATE;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_DRIVER_OPEN_RELEASE:
            eventToSend = EV_DRIVER_OPEN_RELEASED;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_DRIVER_CLOSE_RELEASE:
            eventToSend = EV_DRIVER_CLOSE_RELEASED;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_SECURITY_OPEN_PRESS:
        case EVT_SECURITY_OPEN_AUTO:
            eventToSend = EV_SECURITY_OPEN_GATE;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_SECURITY_CLOSE_PRESS:
        case EVT_SECURITY_CLOSE_AUTO:
            eventToSend = EV_SECURITY_CLOSE_GATE;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_SECURITY_OPEN_RELEASE:
            eventToSend = EV_SECURITY_OPEN_RELEASED;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_SECURITY_CLOSE_RELEASE:
            eventToSend = EV_SECURITY_CLOSE_RELEASED;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_OPEN_LIMIT_PRESS:
            eventToSend = EV_LIMIT_OPENING;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_CLOSED_LIMIT_PRESS:
            eventToSend = EV_LIMIT_CLOSING;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_DRIVER_CONFLICT:
            eventToSend = EV_DRIVER_CONFLICT;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_SECURITY_CONFLICT:
            eventToSend = EV_SECURITY_CONFLICT;
            xQueueSendToBack(evQueue, &eventToSend, portMAX_DELAY);
            break;

        case EVT_OBSTACLE_PRESS:
            xSemaphoreGive(obstacleSemaphore);
            break;

        default:
            break;
    }
}

static void Button_Init(ButtonObject_t *btn,
                        uint8_t (*readFunc)(void),
                        EventType_t pressEvent,
                        EventType_t releaseEvent)
{
    btn->state.lastChangeTime = 0;
    btn->state.isLastEventPress = 0;

    btn->readFunc = readFunc;
    btn->pressEvent = pressEvent;
    btn->releaseEvent = releaseEvent;
}

void InputTask_Init(void)
{
    Button_Init(&driverOpenBtn,
                Read_DriverOpen,
                EVT_DRIVER_OPEN_PRESS,
                EVT_DRIVER_OPEN_RELEASE);

    Button_Init(&driverCloseBtn,
                Read_DriverClose,
                EVT_DRIVER_CLOSE_PRESS,
                EVT_DRIVER_CLOSE_RELEASE);

    Button_Init(&securityOpenBtn,
                Read_SecurityOpen,
                EVT_SECURITY_OPEN_PRESS,
                EVT_SECURITY_OPEN_RELEASE);

    Button_Init(&securityCloseBtn,
                Read_SecurityClose,
                EVT_SECURITY_CLOSE_PRESS,
                EVT_SECURITY_CLOSE_RELEASE);

    Button_Init(&openLimitBtn,
                Read_OpenLimit,
                EVT_OPEN_LIMIT_PRESS,
                EVT_NONE);

    Button_Init(&closedLimitBtn,
                Read_ClosedLimit,
                EVT_CLOSED_LIMIT_PRESS,
                EVT_NONE);

    Button_Init(&obstacleBtn,
                Read_Obstacle,
                EVT_OBSTACLE_PRESS,
                EVT_NONE);
}

static void ProcessButton(ButtonObject_t *btn)
{
    TickType_t now = xTaskGetTickCount();

    // Last event was release
    if (btn->state.isLastEventPress == 0)
    {
        btn->state.isLastEventPress = 1;
        SendEvent(btn->pressEvent);
    }
    // Last event was press
    else
    {
        TickType_t duration = now - btn->state.lastChangeTime;
        btn->state.isLastEventPress = 0;
        if (duration >= pdMS_TO_TICKS(ONE_TOUCH_THRESHOLD_MS)) SendEvent(btn->releaseEvent);
    }
    btn->state.lastChangeTime = now;
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

static uint8_t IsDriverConflict(void)
{
    if (Read_DriverOpen() == 1 && Read_DriverClose() == 1)
    {
        driverOpenBtn.state.isLastEventPress = 1;
        driverCloseBtn.state.isLastEventPress = 1;
        return 1;
    }
    return 0;
}

static uint8_t IsSecurityConflict(void)
{
    if (Read_SecurityOpen() == 1 && Read_SecurityClose() == 1)
    {
        securityOpenBtn.state.isLastEventPress = 1;
        securityCloseBtn.state.isLastEventPress = 1;
        return 1;
    }
    return 0;
}

void vInputTask(void *pvParameters)
{
    ButtonId_t buttonId;
    (void) pvParameters;

    while (1)
    {
        if (xQueueReceive(xButtonInterruptQueue, &buttonId, portMAX_DELAY) == pdPASS)
        {
            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));

            if (IsSecurityConflict())
            {
                SendEvent(EVT_SECURITY_CONFLICT);

                while (IsSecurityConflict())
                {
                    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
                }

                continue;
            }

            if (IsDriverConflict())
            {
                SendEvent(EVT_DRIVER_CONFLICT);

                while (IsDriverConflict())
                {
                    vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
                }

                continue;
            }

            vTaskDelay(pdMS_TO_TICKS(DEBOUNCE_MS));
            ProcessButtonById(buttonId);
        }
    }
}