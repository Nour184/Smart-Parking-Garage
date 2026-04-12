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