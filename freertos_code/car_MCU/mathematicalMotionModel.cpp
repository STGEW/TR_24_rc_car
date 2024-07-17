#include <math.h>

#include "mathematicalMotionModel.h"


// wheel_base - a distance between left and right wheels in meters, f.e. 0.15 m
// wheels_radius - a radius of wheel in meters, f.e. 0.065 m
// odometer_holes_count - total count of holes in a wheel, f.e. 20
DifferentialModel::DifferentialModel(float _wheels_base,
        float _wheels_radius, int _odometer_holes_count) {
    wheels_base = _wheels_base;
    wheels_radius = _wheels_radius;
    odometer_holes_count = odometer_holes_count;
};


// Calculate the state of vehicle
// int delta_n_l - count of measured holes from the left odometer
// int delta_n_r - count of measured holes from the right odometer
// return VehicleState - x,y,phi
VehicleState DifferentialModel::run(int delta_n_l, int delta_n_r) {

    float delta_d_r = calc_delta_d(delta_n_r);
    float delta_d_l = calc_delta_d(delta_n_l);

    float turn_radius = calc_turning_radius(
        delta_d_r, delta_d_l);

    float delta_phi = calc_delta_phi(
        delta_d_r, delta_d_l, turn_radius);

    phi_prev = phi;
    phi += delta_phi;

    float x = calc_x(delta_d_r, delta_d_l, phi_prev, phi, turn_radius);
    float y = calc_y(delta_d_r, delta_d_l, phi_prev, phi, turn_radius);

    VehicleState state = {x, y, phi};

    return state;
}


// reset internal information of the model
void DifferentialModel::reset() {
    phi_prev = 0.0f;
    phi = 0.0f;
};


// delta_n - count of holes measured by odometer
// return delta_d (float) - the length of the path in meters
float DifferentialModel::calc_delta_d(int delta_n) {
    return ((float) delta_n) * 2 * M_PI * wheels_radius / odometer_holes_count;
}

// turning radius. can be called only when delta_d_r != delta_d_l
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// return turning_radius (meters)
float DifferentialModel::calc_turning_radius(float delta_d_r, float delta_d_l) {
    return delta_d_r * wheels_base / (delta_d_l - delta_d_r);
}

// calculate a delta phi for the movement in radians
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// turn_radius (float) - meters, a turning radius   
// return delta_phi (float) - rad, a diff in angle for delta 
float DifferentialModel::calc_delta_phi(float delta_d_r, float delta_d_l, float turn_radius) {
    if (delta_d_l == delta_d_r) {
        return 0.0;
    }
    // delta_d_l != delta_d_r
    if (turn_radius == 0.0f) {
        return delta_d_l / wheels_base;
    } else {
        // turn_radius != 0.0
        return delta_d_r / turn_radius;
    }
}

// at i interval
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// phi_prev (float) - radians, at i-1 of an angle
// phi (float) - radians, at i of an angle
// turn_radius (float) - meters, a turning radius
// return x at interval i (float)
float DifferentialModel::calc_x(
        float delta_d_r, float delta_d_l,
        float phi_prev, float phi,
        float turn_radius) {
    if (delta_d_r == delta_d_l) {
        return sin(phi) * (delta_d_r + delta_d_l) / 2.0;
    }
    return (turn_radius + wheels_base / 2.0) * (
        sin(phi_prev + M_PI / 2.0) + sin(phi - M_PI / 2.0));
}
    
// at i interval
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// phi_prev (float) - radians, at i-1 of an angle
// phi (float) - radians, at i of an angle
// turn_radius (float) - meters, a turning radius
// return y at interval i (float)
float DifferentialModel::calc_y(
        float delta_d_r, float delta_d_l,
        float phi_prev, float phi,
        float turn_radius) {
    if (delta_d_r == delta_d_l) {
        return cos(phi) * (delta_d_r + delta_d_l) / 2.0;
    }
    return (turn_radius + wheels_base / 2.0) * (
            cos(phi_prev + M_PI / 2.0) + cos(phi - M_PI / 2.0));
}