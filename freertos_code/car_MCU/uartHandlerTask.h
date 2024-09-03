#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "commandsParser.h"
#include "../const.h"

// queue to send something to uart
// it contains const char * inside
extern QueueHandle_t q_uart_tx;
extern QueueHandle_t q_target_point_2D;
extern QueueHandle_t q_car_pos;

extern SemaphoreHandle_t uartStopCMDMutex;
extern bool stop_cmd_flag;

// interface for communicating with ext PC
void send_done();
void send_abort();
void send_ok();
void send_position(Vehicle2DPosition &pos);

bool check_new_point_parsed(Point2D &point_2D);
bool stop_cmd_received();

void setupUartHandlerHardware();
void runUartHandlerTask(void *pvParameters);



