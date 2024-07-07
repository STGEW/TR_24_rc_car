#include <stdio.h>
#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"

#include "uartHandlerTask.h"


SemaphoreHandle_t uartMutex;
QueueHandle_t uartTxQueue;

uint8_t uart_buffer[UART_CTRL_BUFF_SIZE];
int uart_buffer_counter = 0;

void setupUartHandlerHardware()
{
    uart_init(UART_CTRL_ID, UART_CTRL_BAUD_RATE);
    gpio_set_function(UART_CTRL_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_CTRL_RX_PIN, GPIO_FUNC_UART);
}

void runUartHandlerTask(void *pvParameters) {
    uint8_t rx_byte;
    size_t bytes_to_read;
    char * uartTxBuff;

    for( ;; )
    {
        // Writing to UART, non-blocking
        if (xQueueReceive(uartTxQueue, &uartTxBuff, 0) == pdPASS) {
            uart_puts(UART_CTRL_ID, uartTxBuff);
        }
        
        // Reading from UART, non-blocking
        if (xSemaphoreTake(uartMutex, 0) == pdTRUE) {
            bytes_to_read = uart_is_readable(UART_CTRL_ID);
            if (bytes_to_read > UART_CTRL_BUFF_SIZE) {
                printf(
                    "WARNING! UART is reading data slower then data appears. "
                    "Bytes to read: %d, buffer size: %d\n",
                    bytes_to_read, UART_CTRL_BUFF_SIZE);
            }
            if (bytes_to_read) {
                if (uart_buffer_counter + bytes_to_read >= UART_CTRL_BUFF_SIZE) {
                    printf(
                        "ERROR! UART RX buffer should store more information than it can\n");
                    printf("Resetting UART RX buffer. The data will be lost\n");
                    uart_buffer_counter = 0;
                }
                for (int i=0; i < bytes_to_read; i++) {
                    uart_buffer[i + uart_buffer_counter] = uart_getc(UART_CTRL_ID);
                }
            }
            xSemaphoreGive(uartMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
