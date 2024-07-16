#include "utils.h"
#include "FreeRTOS.h"
#include "task.h"


float secondsSinceLastTick(TickType_t lastTick) {
    TickType_t diffTicks = xTaskGetTickCount() - lastTick;
    float diffSec = (float) diffTicks  / (float) configTICK_RATE_HZ ;
    return diffSec;
};