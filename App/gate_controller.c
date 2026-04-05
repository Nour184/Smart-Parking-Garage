/**
 * @file gate_controller.c
 * @brief Main Gate Control Task (Medium Priority) - TEAM 2.
 * * TO DO:
 * - Implement the FreeRTOS task loop for the gate.
 * - Read from the IPC Queue to get button events.
 * - Use the Mutex to lock the shared gate state.
 * - Pass the received events into the FSM logic to determine the next action.
 */