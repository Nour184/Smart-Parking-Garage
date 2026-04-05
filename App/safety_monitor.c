/**
 * @file safety_monitor.c
 * @brief Safety and Obstacle Detection Task (Highest Priority) - TEAM 3.
 * * TO DO:
 * - Implement a FreeRTOS task that waits (blocks) on the Obstacle Semaphore.
 * - When semaphore is given, instantly override current FSM state.
 * - Force the 0.5-second reverse logic (REVERSING state) and then stop completely.
 */