#ifndef RF_TX_TASK_H
#define RF_TX_TASK_H

#include "FreeRTOS.h"
#include "semphr.h"

// contains raw measurements from joystick
struct __attribute__((__packed__)) JoystickRawData {
    uint x;
    uint y;
};

extern SemaphoreHandle_t rfTxSemaphore;
extern struct JoystickRawData joystick;

#define TX_LED_TIMEOUT_SEC    1.0


#endif /* RF_TX_TASK_H */