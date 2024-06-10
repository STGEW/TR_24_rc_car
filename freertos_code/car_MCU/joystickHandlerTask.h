#ifndef JOYSTICK_HANDLER_TASK_H
#define JOYSTICK_HANDLER_TASK_H

#include "FreeRTOS.h"
#include "semphr.h"

#define DEAD_ZONE               50
#define DEAD_ZONE_START         2048 - DEAD_ZONE
#define DEAD_ZONE_END           2048 + DEAD_ZONE
// this coefficient is chosen after several experiments
#define TURN_COEF               0.60
#define ADC_TO_PERCENTAGE       100.0 / 2048.0
#define JOYSTICK_TIMEOUT_SEC    1.0

/*
What we get from joystick?
0..4095 as a full range.

Regarding joystick position:
0..2047 - left
2048..4095 - right

How we interpret these values?
2048 - DEAD_ZONE ... 2048 + DEAD_ZONE - safe zone / dead zone / neutral

0 ... 2048 - DEAD_ZONE - 'backward'/'left'
2048 + DEAD_ZONE ... 4095 - 'forward'/'right'
*/

// contains raw measurements from joystick
struct __attribute__((__packed__)) JoystickRawData {
    uint x;
    uint y;
};

extern SemaphoreHandle_t joystickHandlerSemaphore;
extern struct JoystickRawData joystick;

#endif /* JOYSTICK_HANDLER_TASK_H */