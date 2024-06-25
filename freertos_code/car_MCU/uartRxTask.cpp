#include <stdio.h>
#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"
#include "msgParser.h"

#include "inputResolverTask.h"
#include "../const.h"

Parser parser = Parser();


void prvSetupUartHandlerHardware()
{
    uart_init(UART_CTRL_ID, UART_CTRL_BAUD_RATE);
    gpio_set_function(UART_CTRL_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_CTRL_RX_PIN, GPIO_FUNC_UART);
}


void uartHandlerTask(void *pvParameters) {
    uint8_t rx_byte;
    for( ;; )
    {
        // Read a byte from UART
        if (uart_is_readable(UART_CTRL_ID)) {
            rx_byte = uart_getc(UART_CTRL_ID);
            // Process received byte, for example, print it
            printf("Received: %c\n", rx_byte);
            if (xSemaphoreTake(inputResolverMutex, portMAX_DELAY) == pdTRUE) {
                bool res = parser.parseMessage(rx_byte, &message);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust delay as needed
    }
}