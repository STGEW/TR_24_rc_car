#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

#include "../const.h"


extern SemaphoreHandle_t odometerMutex;

void read_odometer_data(uint32_t & _odo_left_count, uint32_t & _odo_right_count);
