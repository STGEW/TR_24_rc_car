#include <stdio.h>
#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"

#include "uartHandlerTask.h"

QueueHandle_t queue_uart_tx;
QueueHandle_t queue_points_2D;
SemaphoreHandle_t uartStopCMDMutex;

static const char * resp_ok = "ok";
static const char * resp_done = "done";
static const char * resp_abort = "abort";

bool stop_cmd_flag = false;

void send_done() {
    if (xQueueSend(queue_uart_tx, &resp_done, portMAX_DELAY) != pdPASS) {
        printf("Error! Can't send 'done' to UART queue\n");
    }
}

void send_abort() {
    if (xQueueSend(queue_uart_tx, &resp_abort, portMAX_DELAY) != pdPASS) {
        printf("Error! Can't send 'abort' to UART queue\n");
    }
}

void send_ok() {
    if (xQueueSend(queue_uart_tx, &resp_ok, portMAX_DELAY) != pdPASS) {
        printf("Error! Can't send 'ok' to UART queue\n");
    }
}

bool check_new_point_parsed(Point2D &point_2D) {
    if (xQueueReceive(queue_points_2D, &point_2D, 0) == pdPASS) {
        printf(
            "Reading new point x: %f y: %f from queue\n",
            point_2D.x, point_2D.y);
        return true;
    }
    return false;
}

bool stop_cmd_received() {
    bool flag = false;
    if (xSemaphoreTake(uartStopCMDMutex, portMAX_DELAY) == pdTRUE) {
        flag = stop_cmd_flag; stop_cmd_flag = false;
        xSemaphoreGive(uartStopCMDMutex);
    }
    return flag;
}

void new_point_cb(Point2D * p) {
    printf("New point cb called with values: x: %f y: %f\n", p->x, p->y);
    if (xQueueSend(queue_points_2D, p, portMAX_DELAY) != pdPASS) {
        printf(
            "Error! Can't send new point x: %f y: %f to queue\n",
            p->x, p->y);
        send_ok();
    }
}

void stop_cb() {
    if (xSemaphoreTake(uartStopCMDMutex, portMAX_DELAY) == pdTRUE) {
        printf("Stop callback called\n");
        stop_cmd_flag = true;
        xSemaphoreGive(uartStopCMDMutex);
        send_ok();
    }
}

void setupUartHandlerHardware()
{
    uart_init(UART_CTRL_ID, UART_CTRL_BAUD_RATE);
    gpio_set_function(UART_CTRL_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_CTRL_RX_PIN, GPIO_FUNC_UART);
}

void runUartHandlerTask(void *pvParameters) {
    size_t bytes_to_read;
    char * uart_tx_ptr;
    void (*new_point_cb_ptr)(Point2D * point_2D) = &new_point_cb;
    void (*stop_cb_ptr)(void) = &stop_cb;

    CommandsParser cmds_parser = CommandsParser(new_point_cb_ptr, stop_cb_ptr);
    printf("Starting UART handler task\n");
    for( ;; )
    {
        // Writing to UART, non-blocking
        if (xQueueReceive(queue_uart_tx, &uart_tx_ptr, 0) == pdPASS) {
            printf("UART task received something from Queue\n");
            uart_puts(UART_CTRL_ID, uart_tx_ptr);
        }

        // Reading from UART, non-blocking
        bytes_to_read = uart_is_readable(UART_CTRL_ID);
        if (0 != bytes_to_read) {
            for (int i = 0; i < bytes_to_read; i++) {
                cmds_parser.parse(uart_getc(UART_CTRL_ID));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
