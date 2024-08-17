#pragma once

#include "FreeRTOS.h"
#include "../const.h"


void setupControlArbiterTask( void );

void runControlArbiterTask( void *pvParameters );
