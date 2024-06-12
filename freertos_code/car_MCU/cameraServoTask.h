#ifndef CAMERA_SERVO_TASK_H
#define CAMERA_SERVO_TASK_H

#include "FreeRTOS.h"
#include "semphr.h"

// a semaphore to communicate with cameraAngle variable
extern SemaphoreHandle_t cameraServoSemaphore;
// an angle for camera to rotate
extern int cameraAngle;

#endif /* CAMERA_SERVO_TASK_H */