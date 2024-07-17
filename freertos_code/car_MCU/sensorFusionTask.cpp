#include <math.h>
#include <stdio.h>

#include "sensorFusionTask.h"
#include "odometerTask.h"
#include "imuRawTask.h"


SemaphoreHandle_t sensorFusionMutex;


static SensorsData _sensors_data = SensorsData {0.0, 0.0, 0.0, 0.0, 0.0};

static uint32_t _odometer_left_count = 0;
static uint32_t _odometer_right_count = 0;

static float _pos[3];
static float _vel[3];
static float _angle[3];
static float _angle_speed[3];

float _calc_odom_dist_m(uint32_t _odom_counter);
void _reset_internal_state();


void read_fused_sensors_data( SensorsData & sensors_data ) {
    // safely get the data
    if (pdTRUE == xSemaphoreTake(sensorFusionMutex, portMAX_DELAY)) {
        sensors_data = _sensors_data;
        _reset_internal_state();
        xSemaphoreGive(sensorFusionMutex);
    }
}

void reset_sensor_fusion_task() {
    if (pdTRUE == xSemaphoreTake(sensorFusionMutex, portMAX_DELAY)) {
        _reset_internal_state();
        xSemaphoreGive(sensorFusionMutex);
    }
}

void runSensorFusionTask(void *pvParameters) {

    float left_wheel_dist_m = 0.0f;
    float right_wheel_dist_m = 0.0f;

    for( ;; ) {

        read_odometer_data(_odometer_left_count, _odometer_right_count);

        left_wheel_dist_m = _calc_odom_dist_m(_odometer_left_count);
        right_wheel_dist_m = _calc_odom_dist_m(_odometer_right_count);

        // printf(
        //     "Odometer values are l: %d %.5f r: %d %.5f\n",
        //     _odometer_left_count, left_odom_dist_m,
        //     _odometer_right_count, right_odom_dist_m);

        read_imu_data(_pos, _vel, _angle, _angle_speed);

        // printf("pos x: %.6f y: %.6f z: %.6f vel x: %.6f y: %.6f z: %.6f\n",
        //     _pos[0], _pos[1], _pos[2], _vel[0], _vel[1], _vel[2]);
/*
        printf("angle x: %f y: %f z: %f\n",
            _angle[0], _angle[1], _angle[2]);
        printf("angle speed x: %f y: %f z: %f\n",
            _angle_speed[0], _angle_speed[1], _angle_speed[2]);
*/
        if (pdTRUE == xSemaphoreTake(sensorFusionMutex, portMAX_DELAY)) {
            // https://www.researchgate.net/publication/228845879_Basic_motion_model_of_autonomous_ground_vehicle
            // TBD - improve algorithm
            // если обе стороны крутятся на d, то по y мы проедем d
            // если одна на d, другая на -d, то мы вообще никуда не проедем
            // если правая на d, другая на 0.
            _sensors_data.d_x += _pos[0];
            _sensors_data.d_y += _pos[1];
            // that's initial way - use IMU for coordinates
            // _sensors_data.d_x += _pos[0];
            // _sensors_data.d_y += _pos[1];
            _sensors_data.d_vx += _vel[0];
            _sensors_data.d_vy += _vel[1];
            _sensors_data.d_heading = +_angle[0];
            // printf(
            //     "dx: %.6f dy: %.6f d_vx: %.6f d_vy: %.6f d_heading: %.6f\n",
            //     _sensors_data.d_x, _sensors_data.d_y,
            //     _sensors_data.d_vx, _sensors_data.d_vy,
            //     _sensors_data.d_heading);

            xSemaphoreGive(sensorFusionMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}


// This method calculates from count of recorded rotations a distance
float _calc_odom_dist_m(uint32_t _odom_counter) {
    // multiplier 2 doesn't have mathematical meaning
    // I've put it there because with current implementation I got
    // twice more than I've expected interrupts from odometer sensors.
    // I've troubleshooted it but didn't find the reason
    // I've tried
    // 1) to change GPIO_IRQ_EDGE_FALL to EDGE_RISE - no success
    // 2) to add external pull up resistor - 10k, 20k - no success
    // 3) to play with timings, and tried to filter it somehow - no success
    // The sensor itself works properly. It gives 3.3V when there is no hole
    // And 0V when there is a hole.
    // I suspect this can be 'contact bounce' which in combination
    // with timings gives doubled value. I'll try to check it with oscilloscope
    return M_PI * WHEEL_DIAMETER_M * _odom_counter / (
        ODOMETER_COUNT_OF_HOLES_IN_DISK * 2.0);
}

void _reset_internal_state() {
    _sensors_data.d_x = 0.0; _sensors_data.d_y = 0.0;
    _sensors_data.d_vx = 0.0; _sensors_data.d_vy = 0.0;
    _sensors_data.d_heading = 0.0;
}
