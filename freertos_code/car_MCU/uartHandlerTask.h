#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

#include "../const.h"

// you can work with uart buffer only when get this mutex
extern SemaphoreHandle_t uartMutex;
// queue to send something to uart
extern QueueHandle_t uartTxQueue;
// buffer to get received from UART data
extern uint8_t uart_buffer[UART_CTRL_BUFF_SIZE];

extern int uart_buffer_counter;

void setupUartHandlerHardware();
void runUartHandlerTask(void *pvParameters);

