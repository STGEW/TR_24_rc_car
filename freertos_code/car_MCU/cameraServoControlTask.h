#ifndef CAMERA_SERVO_CONTROL_TASK_H
#define CAMERA_SERVO_CONTROL_TASK_H

#include "FreeRTOS.h"
#include "semphr.h"

#define CAMERA_SERVO_PIN               14


extern SemaphoreHandle_t cameraServoControlSemaphore;
extern int cameraAngle;

#endif /* CAMERA_SERVO_CONTROL_TASK_H */