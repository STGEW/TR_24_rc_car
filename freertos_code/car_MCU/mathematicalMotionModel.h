#pragma once

// here is a theory about that
// https://www.researchgate.net/publication/228845879_Basic_motion_model_of_autonomous_ground_vehicle
struct VehicleState {
    float x;
    float y;
    float phi;
};

class DifferentialModel{

public:
    // wheel_base - a distance between left and right wheels in meters, f.e. 0.15 m
    // wheels_radius - a radius of wheel in meters, f.e. 0.065 m
    // odometer_holes_count - total count of holes in a wheel, f.e. 20
    DifferentialModel(
        float _wheels_base, float _wheels_radius, int _odometer_holes_count);

    // Calculate the state of vehicle
    // int delta_n_l - count of measured holes from the left odometer
    // int delta_n_r - count of measured holes from the right odometer
    // return VehicleState - x,y,phi
    void update(int delta_n_l, int delta_n_r, VehicleState &state);
    // reset internal information of the model
    void reset();

private:

    // delta_n - count of holes measured by odometer
    float calc_delta_d(int delta_n);

    // turining radius. can be called only when delta_d_r != delta_d_l
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // return turning_radius (meters)
    float calc_turning_radius(float delta_d_r, float delta_d_l);

    // calculate a delta phi for the movement in radians
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // turn_radius (float) - meters, a turning radius   
    // return delta_phi (float) - rad, a diff in angle for delta 
    float calc_delta_phi(float delta_d_r, float delta_d_l, float turn_radius);
    
    // at i interval
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // phi_prev (float) - radians, at i-1 of an angle
    // phi (float) - radians, at i of an angle
    // turn_radius (float) - meters, a turning radius
    // return x at interval i (float)
    float calc_x(
        float delta_d_r, float delta_d_l,
        float phi_prev, float phi,
        float turn_radius);
    
    // at i interval
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // phi_prev (float) - radians, at i-1 of an angle
    // phi (float) - radians, at i of an angle
    // turn_radius (float) - meters, a turning radius
    // return y at interval i (float)
    float calc_y(
        float delta_d_r, float delta_d_l,
        float phi_prev, float phi,
        float turn_radius);

    float phi_prev;
    float phi;
    float wheels_base;
    float wheels_radius;
    int odometer_holes_count;
};