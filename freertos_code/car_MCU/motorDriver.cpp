#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "motorDriver.h"
#include <stdio.h>


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
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 4.f);
    pwm_init(slice_num, &config, true);
}


// initialize required for motor pins
void setupMotorDriver( void )
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


void runMotorDriver( DriverControlData & data )
{
    setMotorDirection(AI1_PIN, AI2_PIN, data.direction_A);
    pwm_set_gpio_level(
        PWM_A_PIN, data.duty_cycle_A);

    setMotorDirection(BI1_PIN, BI2_PIN, data.direction_B);
    pwm_set_gpio_level(
        PWM_B_PIN, data.duty_cycle_B);
}
