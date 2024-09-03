#pragma once
#include "../const.h"


struct PathChunk {
    float x1;       // current pos x
    float y1;       // current pos y
    float phi1;     // current yaw
    float x2;       // desired pos x
    float y2;       // desired pos y
};

struct OdometerValues {
    // to compensate yaw this count of holes should be triggered
    int angle_n_l;  // on the left odometer
    int angle_n_r;  // on the right odometer
    int dist_n;     // this is valid for both odometers
    float phi2;     // estimation of new yaw angle at point 2
};

class DifferentialVehicleModel {

public:
    // wheel_base - a distance between left and right wheels in meters, f.e. 0.15 m
    // wheels_radius - a radius of wheel in meters, f.e. 0.065 m
    // odometer_holes_count - total count of holes in a wheel, f.e. 20
    DifferentialVehicleModel(
        float _wheels_base, float _wheels_radius, int _odometer_holes_count);


    // Convert a path chunk x1,y1,phi1,x2,y2 to expected from odometers values
    void path_to_odometer_values(
        PathChunk &path_chunk,
        OdometerValues &odometer_values
    );

    // delta_n is expected position
    // direction information in clockwise flag
    void odo_to_pos_rotate(int delta_n, bool clockwise, Vehicle2DPosition &d_pos);
    
    // we expect delta_n - position and direction info inside
    // forward flag
    void odo_to_pos_linear(
        int delta_n, bool forward,
        float phi, Vehicle2DPosition &d_pos);

private:

    // dist - meters, float
    int dist_diff_to_odo(float dist);

    // returns a odo for right wheel.
    // positive d_yar means position right wheel odo value
    // left odo value = -1 * right odo value
    int yaw_diff_to_odo(float yaw);

    float square(float x);

    // convert dist to count of holes
    int convert_dist_m_to_n(float dist_m);

    // vehicle parameters
    float wheels_base;
    float wheels_radius;
    int odometer_holes_count;
};