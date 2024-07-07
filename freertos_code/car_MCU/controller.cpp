#include "controller.h"
#include <math.h>


void Controller::start(PathPlanningData & data) {
    _pp_data = data;
    heading_error_rad = atan(data.x / data.y);
    state = STATE_WORKING;
}

void Controller::update(EnginesPwr & pwr, SensorsData & sensors_data ) {
    if (STATE_DONE == state) {
        return; // do nothing in that state
    }

    // STATE_WORKING

    // we arrived
    if (euclidean_dist(_pp_data.x, _pp_data.y) <= pos_error_m) {
        state = STATE_DONE;
        return;
    }
    // update pp data with new values

    // TBD - design it
    // compensate a heading error
    // compensate coordinate error
    
}

float Controller::euclidean_dist(float x, float y) {
    return sqrt((x * x) + (y * y));
}
