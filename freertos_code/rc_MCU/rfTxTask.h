#pragma once

#include "FreeRTOS.h"
#include "semphr.h"


extern SemaphoreHandle_t rfTxSemaphore;
extern struct JoystickRawData joystick;

#define TX_LED_TIMEOUT_SEC    1.0
