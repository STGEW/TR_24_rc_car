#include "commandsProcessorTask.h"
#include "motorDriverTask.h"
#include "hardware/gpio.h"
#include <stdio.h>

// semaphore for reading data by commandsProcessor
SemaphoreHandle_t commandsProcessorSemaphore;
struct JoystickRawData joystick = {0, 0};

/*
Contains values describing desired movement of the car
forward can have values -100 (%) ... +100 (%).
    Minus corresponds to reversed movement
turn -100 .. +100
    Minus corresponds to turning left
*/
int turn = 0;
int forward = 0;

int left_side_power = 0;
int right_side_power = 0;


// initialize required for motor pins
void prvCMDProcessorSetup( void )
{
    // intially OFF
    gpio_init(RADIO_LED_PIN);
    gpio_set_dir(RADIO_LED_PIN, GPIO_OUT);
    gpio_put(RADIO_LED_PIN, 0);
}


void commandsProcessorTask( void *pvParameters )
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

        if (xSemaphoreTake(commandsProcessorSemaphore, 0) == pdTRUE) {
            gpio_put(RADIO_LED_PIN, 1);
            lastCmdTicks = xTaskGetTickCount();
            printf(
                "joystick's values for processing are. x: %u, y: %u\n",
                joystick.x,
                joystick.y);

            // centered values
            // forward -= 2048;
            // turn -= 2048;

            // STEP 1 - processing 'dead zones'
            printf("Step 1 - processing 'dead zones'\n");
            if ((joystick.x >= DEAD_ZONE_START) && (joystick.x <= DEAD_ZONE_END)) {
               forward = 0;
            } else {
                forward = joystick.x;
                forward -= 2048;
            }

            if ((joystick.y >= DEAD_ZONE_START) && (joystick.y <= DEAD_ZONE_END)) {
                turn = 0;
            } else {
                turn = joystick.y;
                turn -= 2048;
            }
            /*
                'dead' zones are excluded and our values are centered around 0.
                They can be negative and positive
            */
            printf(
                "After excluding 'dead zones'; forward: %d, turn: %d\n",
                forward, turn);


            /*
                convert our values to -100%...+100%
            */
            printf("Step 2 - convert values to the range -100%...+100%\n");
            forward = forward * FROM_JOYSTICK_COEF;
            turn = turn * FROM_JOYSTICK_COEF;
            printf(
                "After conversion to -100%...+100% range; forward: %d, turn: %d\n",
                forward, turn);

            // STEP 3 - convert joystick input to left side and right side wheels
            printf("Step 3 - convert jostick's values to power lvls for left and right sides\n");

            // first we apply forward/backward:
            left_side_power = forward;
            right_side_power = forward;
            printf(
                "After applying forward values; left_side_power: %d right_side_power: %d\n",
                left_side_power,
                right_side_power);

            if (turn > 0) {
                // turn to the right. Left wheels should rotate faster
                left_side_power += turn * TURN_COEF;
                right_side_power -= turn * TURN_COEF;
            } else if (turn < 0) {
                // turn to the left. Right wheels should rotate faster than left
                left_side_power -= -1 * turn * TURN_COEF;
                right_side_power += -1 * turn * TURN_COEF;
            }
            printf(
                "After applying turn values; left_side_power: %d right_side_power: %d\n",
                left_side_power,
                right_side_power);

            // limit within -100..100 range 
            if (left_side_power > 100) {
                left_side_power = 100;
            } else if (left_side_power < -100) {
                left_side_power = -100;
            }

            if (right_side_power > 100) {
                right_side_power = 100;
            } else if (right_side_power < -100) {
                right_side_power = -100;
            } 
            printf(
                "After limiting within -100..100; left_side_power: %d right_side_power: %d\n",
                left_side_power,
                right_side_power);

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
