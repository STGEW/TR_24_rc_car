#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hardware/gpio.h"
#include <stdio.h>

#include "../const.h"

SemaphoreHandle_t odometerMutex;

// values you may use somewhere/somehow
volatile uint32_t odometer_left_count = 0;
volatile uint32_t odometer_right_count = 0;

TaskHandle_t xOdometerLeftTaskHandle;
TaskHandle_t xOdometerRightTaskHandle;


void read_odometer_data(uint32_t & _odo_left_count, uint32_t & _odo_right_count) {
    if (xSemaphoreTake(odometerMutex, portMAX_DELAY) == pdTRUE) {
        _odo_left_count = odometer_left_count;
        odometer_left_count = 0;
        _odo_right_count = odometer_right_count;
        odometer_right_count = 0;
        xSemaphoreGive(odometerMutex);
    }
}

void odometer_gpio_callback(uint gpio, uint32_t events)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    // Check if the interrupt was triggered by the specific pin
    if (gpio == ODOMETER_LEFT_PIN) {
        vTaskNotifyGiveFromISR(xOdometerLeftTaskHandle, &xHigherPriorityTaskWoken);
    } else if (gpio == ODOMETER_RIGHT_PIN) {
        vTaskNotifyGiveFromISR(xOdometerRightTaskHandle, &xHigherPriorityTaskWoken);
    }
    // Force a context switch if needed
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void setupOdometerTask( void ) {
    auto setup_odometer_pin = [](int pin) -> void {
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_IN);
        gpio_pull_up(pin);
        gpio_set_irq_enabled_with_callback(
            pin,
            GPIO_IRQ_EDGE_RISE,
            true,
            &odometer_gpio_callback);
    };

    setup_odometer_pin(ODOMETER_LEFT_PIN);
    setup_odometer_pin(ODOMETER_RIGHT_PIN);
}

void runOdometerLeftTask( void *pvParameters )
{
    for (;;) {
        // Wait for the notification from the ISR
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // with mutex ?
        if (xSemaphoreTake(odometerMutex, portMAX_DELAY) == pdTRUE) {
            odometer_left_count++;
            xSemaphoreGive(odometerMutex);
        }
        printf(
            "odometer_left_count is: %d\n",
            odometer_left_count);
    }
}

void runOdometerRightTask( void *pvParameters )
{
    for (;;) {
        // Wait for the notification from the ISR
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (xSemaphoreTake(odometerMutex, portMAX_DELAY) == pdTRUE) {
            odometer_right_count++;
            xSemaphoreGive(odometerMutex);
        }
        printf(
            "odometer_right_count is: %d\n",
            odometer_right_count);
    }
}

