#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "../const.h"


// related to rd tranceiver part
extern SemaphoreHandle_t inputResolverRfDataMutex;
extern bool ext_control;
extern struct EnginesPwr rf_engines_pwr;
extern TickType_t lastRFDataTick;

// Related to path planning part
extern SemaphoreHandle_t inputResolverPathPlanningMutex;
extern PathPlanningData inp_res_pp_data;
extern bool pp_updated;
