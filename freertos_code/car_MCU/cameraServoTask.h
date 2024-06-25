#pragma once

#include "FreeRTOS.h"
#include "semphr.h"

// a semaphore to communicate with cameraAngle variable
extern SemaphoreHandle_t cameraServoSemaphore;
// an angle for camera to rotate
extern int cameraAngle;