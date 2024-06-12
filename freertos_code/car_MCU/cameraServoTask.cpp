#include <stdio.h>
#include "cameraServoTask.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "../pins.h"


// variables to communicate with task
int cameraAngle = 90;
SemaphoreHandle_t cameraServoSemaphore;


uint16_t calc_duty_from_angle(int angle) {
    // some magic numbers. I was unable to calculate it in a proper way
    return (uint16_t) (180 + (1456 - 180) * angle / 180);
}


void setupCameraServoTask() {
    // Configure PWM settings
    gpio_set_function(CAMERA_SERVO_PIN, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(CAMERA_SERVO_PIN);
    printf("Camera servo task slice: %u\n", slice_num);
    
    pwm_config config = pwm_get_default_config();
    
    // Set clock divider to achieve desired PWM frequency
    // Formula: PWM clock frequency = sysclock / (divider + 1)
    // For 50 Hz, PWM clock frequency = 125 MHz / (2499 + 1) = 50 kHz
    pwm_config_set_clkdiv(&config, 2499); // 2499 + 1 = 2500
    
    // Set counter wrap value for desired PWM period
    // Formula: PWM period = (wrap + 1) / PWM clock frequency
    // For 50 Hz, PWM period = (2500 + 1) / 50 kHz = 0.02 seconds = 20 ms
    pwm_config_set_wrap(&config, 2500); // 2500 + 1 = 2501

    // Initialize PWM
    pwm_init(slice_num, &config, true);

    uint16_t duty = calc_duty_from_angle(cameraAngle);
    pwm_set_gpio_level(CAMERA_SERVO_PIN, duty);
}


void runCameraServoTask( void *pvParameters ) {
    for( ;; )
    {
        if (xSemaphoreTake(cameraServoSemaphore, 0) == pdTRUE) {
            printf("Camera new angle to set: %d\n", cameraAngle);
            uint16_t duty = calc_duty_from_angle(cameraAngle);
            pwm_set_gpio_level(CAMERA_SERVO_PIN, duty);
        }
        // 10 msec delay ~ 100 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
