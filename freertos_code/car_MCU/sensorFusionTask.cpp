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


void read_fused_sensors_data( SensorsData & sensors_data ) {
    // safely get the data
    if (pdTRUE == xSemaphoreTake(sensorFusionMutex, portMAX_DELAY)) {
        sensors_data = _sensors_data;
        _sensors_data.d_x = 0.0; _sensors_data.d_y = 0.0;
        _sensors_data.d_vx = 0.0; _sensors_data.d_vy = 0.0;
        _sensors_data.d_heading = 0.0;
        xSemaphoreGive(sensorFusionMutex);
    }
}

void reset_sensor_fusion_task() {
    if (pdTRUE == xSemaphoreTake(sensorFusionMutex, portMAX_DELAY)) {
        _sensors_data.d_x = 0.0;
        _sensors_data.d_y = 0.0;
        _sensors_data.d_vx = 0.0;
        _sensors_data.d_vy = 0.0;
        _sensors_data.d_heading = 0.0;
        xSemaphoreGive(sensorFusionMutex);
    }
}

void runSensorFusionTask(void *pvParameters) {
    for( ;; )
    {
        read_odometer_data(_odometer_left_count, _odometer_right_count);
        read_imu_data(_pos, _vel, _angle, _angle_speed);
        if (pdTRUE == xSemaphoreTake(sensorFusionMutex, portMAX_DELAY)) {
            // TBD - improve algorithm
            _sensors_data.d_x += _pos[0];
            _sensors_data.d_y += _pos[1];
            _sensors_data.d_vx += _vel[0];
            _sensors_data.d_vy += _vel[1];
            _sensors_data.d_heading = +_angle[0];
            xSemaphoreGive(sensorFusionMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
