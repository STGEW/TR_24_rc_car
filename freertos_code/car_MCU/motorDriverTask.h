#ifndef DRIVER_TASK_H
#define DRIVER_TASK_H

#include "FreeRTOS.h"
#include "semphr.h"


/*
    USED DRIVER: tb6612fng

    AI1 (BI1)       AI2 (BI2)           Direction
    LOW             LOW                 OFF
    HIGH            LOW                 FORWARD
    LOW             HIGH                REVERSE
    HIGH            HIGH                BRAKE

    STBY PIN - always HIGH
*/

#define FORWARD         1
#define OFF             2
#define REVERSE         3
#define BRAKE           4

#define AI1_PIN         6
#define AI2_PIN         7
#define BI1_PIN         8
#define BI2_PIN         9
#define STBY_PIN        10
#define PWM_A_PIN       11
#define PWM_B_PIN       12

#define DRIVER_COEF     65535 / 100

// duty cycle 0 .. 65 535
struct DriverControlData {
    int direction_A;
    int direction_B;
    int duty_cycle_A;
    int duty_cycle_B;
};

extern SemaphoreHandle_t motorDriverSemaphore;
extern struct DriverControlData driver;

#endif /* DRIVER_TASK_H */