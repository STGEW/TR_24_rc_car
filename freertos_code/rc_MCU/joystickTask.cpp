#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#include "rfTxTask.h"
#include "../pins.h"


void setupJoystickTask( void )
{
    adc_init();
    adc_gpio_init(X_AXIS_PIN);
    adc_gpio_init(Y_AXIS_PIN);
}


void runJoystickTask( void *pvParameters )
{

    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {

        // originally ADC returns values in range: 0 .. 4095
        adc_select_input(ADC_CHANNEL_X);
        joystick.x = adc_read();

        adc_select_input(ADC_CHANNEL_Y);
        joystick.y = adc_read();

        xSemaphoreGive(rfTxSemaphore);
        // 100 msec delay ~ 10 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
