#include <stdio.h>
#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "task.h"

#include "rfRxSimulatorTask.h"
#include "commandsProcessorTask.h"

// 0.. 4095

void prvSetupRFSimulatorHardware()
{
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}




void rfRxSimulatorTask(void *pvParameters) {
    uint8_t rx_byte;
    for( ;; )
    {
        // Read a byte from UART
        if (uart_is_readable(UART_ID)) {
            rx_byte = uart_getc(UART_ID);
            // Process received byte, for example, print it
            printf("Received: %c\n", rx_byte);
            xSemaphoreGive(commandsProcessorSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Adjust delay as needed
    }
}