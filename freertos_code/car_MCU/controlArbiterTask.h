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

// this should be removed. I've just put it here to fix compilation problem
// the class should be heavily reworked
#include "controller.h"
// struct __attribute__((__packed__)) PathPlanningData {
//     float x;
//     float y;
//     float v_x;
//     float v_y;
// };

extern PathPlanningData inp_res_pp_data;
extern bool pp_updated;
