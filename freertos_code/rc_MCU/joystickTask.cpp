#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include <stdio.h>

#include "rfTxTask.h"


#define X_AXIS_PIN      26
#define Y_AXIS_PIN      27
#define ADC_CHANNEL_X   0
#define ADC_CHANNEL_Y   1


// initialize required for motor pins
void prvSetupJoystickHardware( void )
{
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(X_AXIS_PIN);
    adc_gpio_init(Y_AXIS_PIN);
}


void joystickTask( void *pvParameters )
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
        joystick.x = adc_read(); // adc_raw_x
        // printf("joystick x value: %u\n", joystick.x);

        adc_select_input(ADC_CHANNEL_Y);
        joystick.y = adc_read(); // adc_raw_y
        // printf("joystick y value: %u\n", joystick.y);

        xSemaphoreGive(rfTxSemaphore);
        // 100 msec delay ~ 10 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
