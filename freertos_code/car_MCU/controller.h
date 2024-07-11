#pragma once

#include "../const.h"
#include <stdio.h>

#define STATE_DONE 1
#define STATE_WORKING 2

// Controller assumes that X axis is a longitudinal axis of a car
// Y - transverse axis
#define X_AXIS_IDX 0
#define Y_AXIS_IDX 1


class Controller {

public:
    Controller(): _pp_data({0.0, 0.0, 0.0, 0.0}), state(STATE_DONE) {}
    void start(PathPlanningData & data);
    void update(EnginesPwr & pwr, SensorsData & sensors_data );

private:
    float euclidean_dist(float x, float y);

    static const int pos_error_m = 0.1;
    PathPlanningData _pp_data;
    float heading_error_rad = 0.0;
    uint8_t state;
};
