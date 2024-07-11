#include <stdio.h>
#include "hardware/i2c.h"
#include "FreeRTOS.h"
#include "pico/stdlib.h"
#include "semphr.h"
#include "pico/binary_info.h"

#include "../const.h"
#include "../utils.h"
#include "imuRawTask.h"


SemaphoreHandle_t imuMutex;

int addr = 0x68;

float _vel[3] = {0.0, 0.0, 0.0};
float _pos[3]= {0.0, 0.0, 0.0};
float _angle[3] = {0.0, 0.0, 0.0};
float _angle_speed[3] = {0.0, 0.0, 0.0};

void read_imu_data(float * pos, float * vel, float * angle, float * angle_speed) {
    if (xSemaphoreTake(imuMutex, portMAX_DELAY) == pdTRUE) {
        for (int i = 0; i < 3; i++) {
            pos[i] = _pos[i]; _pos[i] = 0;
            vel[i] = _vel[i]; _vel[i] = 0;
            angle[i] = _angle[i]; _angle[i] = 0;
            angle_speed[i] = _angle_speed[i]; _angle_speed[i] = 0;
        }
        xSemaphoreGive(imuMutex);
    }
}
void mpu6050_init() {

    uint8_t buf_reset[] = {0x6B, 0x00};
    i2c_write_blocking(i2c1, addr, buf_reset, 2, true);

    // config accel bits as 00000000 (+- 2g)
    uint8_t buf_accel[] = {0x1C, 0x00};
    i2c_write_blocking(i2c1, addr, buf_accel, 2, true);

    // config gyro bits as 00001000 (+- 500 deg/s)
    uint8_t buf_gyro[] = {0x1B, 0x01};
    i2c_write_blocking(i2c1, addr, buf_gyro, 2, false);
}

void setupIMURawTask() {

 	// This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(IMU_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(IMU_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(IMU_I2C_SDA_PIN);
    gpio_pull_up(IMU_I2C_SCL_PIN);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(IMU_I2C_SDA_PIN, IMU_I2C_SCL_PIN, GPIO_FUNC_I2C));
}

void mpu6050_read_raw(float accel[3], float gyro[3], float *temp) {
    // For this particular device, we send the device the register we want to read
    // first, then subsequently read from the device. The register is auto incrementing
    // so we don't need to keep sending the register we want, just the first.

    uint8_t buffer[6];

    // Start reading acceleration registers from register 0x3B for 6 bytes
    uint8_t val = 0x3B;
    i2c_write_blocking(i2c1, addr, &val, 1, true); // true to keep master control of bus
    i2c_read_blocking(i2c1, addr, buffer, 6, false);

    for (int i = 0; i < 3; i++) {
        int16_t val = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);

        accel[i] = (float) val;
        accel[i] = accel[i] / 16384.0;
    }

    // Now gyro data from reg 0x43 for 6 bytes
    // The register is auto incrementing on each read
    val = 0x43;
    i2c_write_blocking(i2c1, addr, &val, 1, true);
    i2c_read_blocking(i2c1, addr, buffer, 6, false);  // False - finished with bus

    for (int i = 0; i < 3; i++) {
        int16_t val = (buffer[i * 2] << 8 | buffer[(i * 2) + 1]);
        gyro[i] = (float) val;
        gyro[i] = gyro[i] / 65.5;
    }

    // Now temperature from reg 0x41 for 2 bytes
    // The register is auto incrementing on each read
    val = 0x41;
    i2c_write_blocking(i2c1, addr, &val, 1, true);
    i2c_read_blocking(i2c1, addr, buffer, 2, false);  // False - finished with bus

    val = (buffer[0] << 8 | buffer[1]);
    *temp = ((float) val);
    *temp = (*temp / 340.0) + 36.53;
}

void runIMURawTask( void *pvParameters ) 
{

    TickType_t xNextWakeTime = xTaskGetTickCount();
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    TickType_t last_read_tick = 0;

    float acceleration[3], gyro[3], temp;
    float sec_since_last_read = 0.0;

    mpu6050_init();

    for( ;; )
    {
        mpu6050_read_raw(acceleration, gyro, &temp);
        // TBD - wait mutex forever
        if (0 == last_read_tick) {
            // first call
            last_read_tick = xTaskGetTickCount();
        } else {
            if (xSemaphoreTake(imuMutex, portMAX_DELAY) == pdTRUE) {
                sec_since_last_read = secondsSinceLastTick(last_read_tick);
                for (int i = 0; i++; i < 3) {
                    _vel[i] = _vel[i] + sec_since_last_read * acceleration[i];
                    _pos[i] = _pos[i] + sec_since_last_read * _vel[i];
                    printf("vel [%d]: %f, pos [%d]: %f\n",
                        i, _vel[i], 
                        i, _pos[i]);
                    // angle_speed[i] = angle_speed[i] + gyro[i];
                    // TBD - how to update angle speed and angle from gyro
                }
                xSemaphoreGive(imuMutex);
            }
            last_read_tick = xTaskGetTickCount();
        }
        

        // These are the raw numbers from the chip, so will need tweaking to be really useful.
        // See the datasheet for more information
        // printf("Acc. X = %f, Y = %f, Z = %f\n", acceleration[0], acceleration[1], acceleration[2]);
        // printf("Gyro. X = %f, Y = %f, Z = %f\n", gyro[0], gyro[1], gyro[2]);
        // Temperature is simple so use the datasheet calculation to get deg C.
        // Note this is chip temperature.
        // printf("Temp. = %f\n", temp);

        // 10 msec delay ~ 1 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
