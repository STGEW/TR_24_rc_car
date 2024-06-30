#include "inputResolverTask.h"
#include "motorDriverTask.h"
#include "hardware/gpio.h"
#include <stdio.h>

SemaphoreHandle_t inputResolverMutex;

bool ext_control = false;
struct EnginesPwr rf_engines_pwr = {0, 0};
TickType_t lastRFDataTick = 0.0;

struct EnginesPwr uart_engines_pwr;


void dataEnginesToDriver(EnginesPwr & engines, DriverControlData & driver);


// initialize required for motor pins
void setupInputResolverTask( void )
{
    // intially OFF
    gpio_init(RADIO_LED_PIN);
    gpio_set_dir(RADIO_LED_PIN, GPIO_OUT);
    gpio_put(RADIO_LED_PIN, 0);
}


void runInputResolverTask( void *pvParameters )
{
    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    TickType_t lastCmdTicks = xTaskGetTickCount();
    TickType_t sinceLastCmdTicks = 0;
    float sinceLastCmdSec = 0.0;

    bool _ext_control;
    struct EnginesPwr _rf_engines_pwr;
    TickType_t _lastRFDataTick;

    struct EnginesPwr _uart_engines_pwr;

    auto secondsSinceLastTick = [](TickType_t lastTick) -> float {
        TickType_t diffTicks = xTaskGetTickCount() - lastTick;
        float diffSec = diffTicks / configTICK_RATE_HZ;
        return diffSec;
    };

    for( ;; )
    {
        // just polling
        if (xSemaphoreTake(inputResolverMutex, 0) == pdTRUE) {
            _ext_control = ext_control;
            _rf_engines_pwr = rf_engines_pwr;
            _lastRFDataTick = lastRFDataTick;

            _uart_engines_pwr = uart_engines_pwr;
            xSemaphoreGive(inputResolverMutex);
        }

        float sec = secondsSinceLastTick(_lastRFDataTick);
        if (sec > JOYSTICK_TIMEOUT_SEC) {
            // no data from joystick since reasonable amount of time
            gpio_put(RADIO_LED_PIN, 0);
            // stopping the car
            driver.direction_A = OFF;
            driver.direction_B = OFF;
        } else {
            gpio_put(RADIO_LED_PIN, 1);
            // fresh data from joystick
            if (_ext_control) {
                dataEnginesToDriver(_uart_engines_pwr, driver);
            } else {
                dataEnginesToDriver(_rf_engines_pwr, driver);
            }
        }
        // printf(
        //     "motor A direction is: %d duty cycle: %d\n",
        //     driver.direction_A,
        //     driver.duty_cycle_A);
        // printf(
        //     "motor B direction is: %d duty cycle: %d\n",
        //     driver.direction_B,
        //     driver.duty_cycle_B);

        xSemaphoreGive(motorDriverSemaphore);

        // 50 msec delay ~ 20 Hz
        vTaskDelay(pdMS_TO_TICKS( 50 ));
    }
}

void dataEnginesToDriver(EnginesPwr & engines, DriverControlData & driver) {
    auto helper = [](int32_t e, int & dir, int & duty) -> void {
        if (e == 0) {
            dir = OFF;
            duty = 0;
        } else if (e > 0) {
            dir = FORWARD;
            duty = e * DRIVER_COEF;
        } else if (e < 0) {
            dir = REVERSE;
            duty = -1 * e * DRIVER_COEF;
        }
    };

    helper(engines.left, driver.direction_A, driver.duty_cycle_A);
    helper(engines.right, driver.direction_B, driver.duty_cycle_B);
}
