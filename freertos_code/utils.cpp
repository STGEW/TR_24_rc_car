#include "utils.h"
#include "FreeRTOS.h"
#include "task.h"


// TBD - move it to some shared place
float secondsSinceLastTick(TickType_t lastTick) {
    TickType_t diffTicks = xTaskGetTickCount() - lastTick;
    float diffSec = diffTicks / configTICK_RATE_HZ;
    return diffSec;
};