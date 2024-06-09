#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "motorDriverTask.h"
#include <stdio.h>


// semaphore for reading driver control
SemaphoreHandle_t motorDriverSemaphore;
// contains values for driver
struct DriverControlData driver = {OFF, OFF, 0, 0};


void setMotorDirection(int I1_PIN, int I2_PIN, int direction) {
    switch (direction) {
        case FORWARD:
            gpio_put(I1_PIN, 1);
            gpio_put(I2_PIN, 0);
            break;
        case OFF:
            gpio_put(I1_PIN, 0);
            gpio_put(I2_PIN, 0);
            break ;
        case REVERSE:
            gpio_put(I1_PIN, 0);
            gpio_put(I2_PIN, 1);
            break;
        case BRAKE:
            gpio_put(I1_PIN, 1);
            gpio_put(I2_PIN, 1);
            break;
        default:
            printf("For motor direction unknown value arrived: %d\n", direction);
            break;
    }
}

void pwm_init_pin(uint8_t pin) {
    gpio_set_function(pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(pin);
    printf("Motor driver task slice: %u\n", slice_num);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num, &config, true);
}


// initialize required for motor pins
void prvSetupDriverHardware( void )
{
    // intially OFF
    gpio_init(AI1_PIN);
    gpio_init(AI2_PIN);
    gpio_set_dir(AI1_PIN, GPIO_OUT);
    gpio_set_dir(AI2_PIN, GPIO_OUT);
    setMotorDirection(AI1_PIN, AI2_PIN, OFF);

    gpio_init(BI1_PIN);
    gpio_init(BI2_PIN);
    gpio_set_dir(BI1_PIN, GPIO_OUT);
    gpio_set_dir(BI2_PIN, GPIO_OUT);
    setMotorDirection(BI1_PIN, BI2_PIN, OFF);

    // STDBY is always ON
    gpio_init(STBY_PIN);
    gpio_set_dir(STBY_PIN, GPIO_OUT);
    gpio_put(STBY_PIN, 1);

    // setup PWM A and PWM B
    pwm_init_pin(PWM_A_PIN);
    pwm_init_pin(PWM_B_PIN);
}


void driverTask( void *pvParameters )
{

    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    setMotorDirection(AI1_PIN, AI2_PIN, OFF);
    setMotorDirection(BI1_PIN, BI2_PIN, OFF);
    
    for( ;; )
    {
        if (xSemaphoreTake(motorDriverSemaphore, 0) == pdTRUE) {
            printf(
                "motor A direction is: %d duty cycle: %d\n",
                driver.direction_A,
                driver.duty_cycle_A);
            setMotorDirection(AI1_PIN, AI2_PIN, driver.direction_A);
            //gpio_put(PWM_A_PIN, 1);
            pwm_set_gpio_level(
                PWM_A_PIN, driver.duty_cycle_A);

            printf(
                "motor B direction is: %d duty cycle: %d\n",
                driver.direction_B,
                driver.duty_cycle_B);
            setMotorDirection(BI1_PIN, BI2_PIN, driver.direction_B);
            // gpio_put(PWM_B_PIN, 1);
            pwm_set_gpio_level(
                PWM_B_PIN, driver.duty_cycle_B);
        } else {
            // printf("motorDriverTask didn't get a semaphore \n");
        }
        
        // 10 msec delay ~ 100 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
