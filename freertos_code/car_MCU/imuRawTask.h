#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

#include "../const.h"


extern SemaphoreHandle_t imuMutex;

void read_imu_data(float * pos, float * vel, float * angle, float * angle_speed);
