#include <stdio.h>
#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"

#include "enginesDataParser.h"
#include "inputResolverTask.h"
#include "../const.h"

EnginesDataParser parser = EnginesDataParser();

uint8_t uart_buffer[UART_CTRL_BUFF_SIZE];

void setupUartRxHardware()
{
    uart_init(UART_CTRL_ID, UART_CTRL_BAUD_RATE);
    gpio_set_function(UART_CTRL_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_CTRL_RX_PIN, GPIO_FUNC_UART);
}


void runUartRxTask(void *pvParameters) {
    uint8_t rx_byte;
    size_t bytes_to_read;
    for( ;; )
    {
        // Read a byte from UART
        bytes_to_read = uart_is_readable(UART_CTRL_ID);
        if (bytes_to_read > UART_CTRL_BUFF_SIZE) {
            printf(
                "WARNING! UART is reading data slower then data appears. "
                "Bytes to read: %d, buffer size: %d\n",
                bytes_to_read, UART_CTRL_BUFF_SIZE);
        }
        if (bytes_to_read) {
            for (int i=0; i < bytes_to_read; i++) {
                uart_buffer[i] = uart_getc(UART_CTRL_ID);
                // Process received byte, for example, print it
            }
            if (xSemaphoreTake(inputResolverMutex, portMAX_DELAY) == pdTRUE) {
                for (int i=0; i < bytes_to_read; i++) {
                    if (parser.parse(uart_buffer[i], uart_engines_pwr)) {
                        lastRFDataTick = xTaskGetTickCount();
                    }
                }
                xSemaphoreGive(inputResolverMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust delay as needed
    }
}