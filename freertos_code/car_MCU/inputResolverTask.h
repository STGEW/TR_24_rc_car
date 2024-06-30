#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "../const.h"


extern SemaphoreHandle_t inputResolverMutex;

extern bool ext_control;
extern struct EnginesPwr rf_engines_pwr;
extern TickType_t lastRFDataTick;

extern struct EnginesPwr uart_engines_pwr;
extern TickType_t lastUARTDataTick;
