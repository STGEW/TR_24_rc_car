#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

#include "../const.h"


extern SemaphoreHandle_t sensorFusionMutex;

void read_fused_sensors_data( SensorsData & sensors_data );
void reset_sensor_fusion_task();
void runSensorFusionTask(void *pvParameters);
