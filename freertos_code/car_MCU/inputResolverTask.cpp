#include "inputResolverTask.h"
#include "motorDriverTask.h"
#include "hardware/gpio.h"
#include <stdio.h>

SemaphoreHandle_t inputResolverSemaphore;

bool ext_control = false;
struct EnginesPwr rf_engines_pwr = {0, 0};
TickType_t lastRFDataTick = 0.0;
bool rfDataFresh = false;

struct EnginesPwr uart_engines_pwr;
TickType_t lastUARTDataTick = 0.0;
bool uartDataFresh = false;


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

    for( ;; )
    {

        if (xSemaphoreTake(inputResolverSemaphore, 0) == pdTRUE) {
            gpio_put(RADIO_LED_PIN, 1);
            lastCmdTicks = xTaskGetTickCount();
            printf(
                "joystick's values for processing are. x: %u, y: %u\n",
                joystick.x,
                joystick.y);

            // centered values
            // forward -= 2048;
            // turn -= 2048;

           

            // Step 3 - convert powers to what our driver expects
            printf(
                "Step 3. Convert power values to what our driver is expecting.\n");
            if (left_side_power == 0) {
                driver.direction_A = OFF;
                driver.duty_cycle_A = 0;
            } else if (left_side_power > 0) {
                driver.direction_A = FORWARD;
                driver.duty_cycle_A = left_side_power * DRIVER_COEF;
            } else if (left_side_power < 0) {
                driver.direction_A = REVERSE;
                driver.duty_cycle_A = -1 * left_side_power * DRIVER_COEF;
            }

            if (right_side_power == 0) {
                driver.direction_B = OFF;
                driver.duty_cycle_B = 0;
            } else if (right_side_power > 0) {
                driver.direction_B = FORWARD;
                driver.duty_cycle_B = right_side_power * DRIVER_COEF;
            } else if (right_side_power < 0) {
                driver.direction_B = REVERSE;
                driver.duty_cycle_B = -1 * right_side_power * DRIVER_COEF;
            }
            printf(
                "motor A direction is: %d duty cycle: %d\n",
                driver.direction_A,
                driver.duty_cycle_A);
            printf(
                "motor B direction is: %d duty cycle: %d\n",
                driver.direction_B,
                driver.duty_cycle_B);

            xSemaphoreGive(motorDriverSemaphore);

        } else {
            sinceLastCmdTicks = xTaskGetTickCount() - lastCmdTicks;
            sinceLastCmdSec = sinceLastCmdTicks / configTICK_RATE_HZ;
            if (sinceLastCmdSec > JOYSTICK_TIMEOUT_SEC) {
                driver.direction_A = OFF;
                driver.direction_B = OFF;
                xSemaphoreGive(motorDriverSemaphore);
                lastCmdTicks = xTaskGetTickCount();
                gpio_put(RADIO_LED_PIN, 0);
            }
            // printf("commandsProcessorTask didn't get a semaphore \n");
        }

        // pdMS_TO_TICKS( 1000 )
        // 10 msec delay ~ 100 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
