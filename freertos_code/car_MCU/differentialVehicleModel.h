#pragma once

// here is a theory about that
// https://www.researchgate.net/publication/228845879_Basic_motion_model_of_autonomous_ground_vehicle

struct Vehicle2DPosition {
    float x;
    float y;
    float phi;
};

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

    // OBSOLETE
    // Calculate the position of vehicle
    // int delta_n_r - count of measured holes from the right odometer
    // int delta_n_l - count of measured holes from the left odometer
    // return Vehicle2DPosition - x,y,phi
    // void odometer_to_position_diff(
    //     int delta_n_r, int delta_n_l,
    //     Vehicle2DPosition &position_diff);



    // OBSOLETE - no more internal state
    // reset internal information of the model
    // void reset_internal_state();

private:

    /*
    ######################################################################################
    # Methods which are used to calculate from odometer's data a position of the vehicle #
    # odometer_to_position_diff - is an interface                                        #
    ######################################################################################
    */
    // OBSOLETE
    // delta_n - count of holes measured by odometer
    // float calc_delta_d(int delta_n);

    // OBSOLETE
    // turning radius. can be called only when delta_d_r != delta_d_l
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // return turning_radius (meters)
    // float calc_turning_radius(float delta_d_r, float delta_d_l);

    // OBSOLETE
    // calculate a delta phi for the movement in radians
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // turn_radius (float) - meters, a turning radius   
    // return delta_phi (float) - rad, a diff in angle for delta 
    // float calc_delta_phi(float delta_d_r, float delta_d_l, float turn_radius);
    
    // OBSOLETE
    // at i interval
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // phi_prev (float) - radians, at i-1 of an angle
    // phi (float) - radians, at i of an angle
    // turn_radius (float) - meters, a turning radius
    // return x at interval i (float)
    // float calc_x(
    //     float delta_d_r, float delta_d_l,
    //     float phi_prev, float phi,
    //     float turn_radius);
    
    // OBSOLETE
    // at i interval
    // delta_d_r (float) - meters of movement right side
    // delta_d_l (float) - meters of movement left side
    // phi_prev (float) - radians, at i-1 of an angle
    // phi (float) - radians, at i of an angle
    // turn_radius (float) - meters, a turning radius
    // return y at interval i (float)
    // float calc_y(
    //     float delta_d_r, float delta_d_l,
    //     float phi_prev, float phi,
    //     float turn_radius);

    // dist - meters, float
    int dist_diff_to_odo(float dist);

    // returns a odo for right wheel.
    // positive d_yar means position right wheel odo value
    // left odo value = -1 * right odo value
    int yaw_diff_to_odo(float yaw);

    /*
    ######################################################################################
    # Methods which are used to convert a path chunk into expected from odometer values  #
    # path_to_odometer_values - is an interface                                          #
    ######################################################################################
    */

    float square(float x);

    // OBSOLETE
    // step 1 - change phi angle
    // void convert_delta_phi_to_n(
    //     float delta_phi, int &angle_n_l, int &angle_n_r);

    // OBSOLETE
    // step 2 - change dist
    // void convert_delta_phi_to_dist_m(
    //     float delta_phi, float &dist_l_m, float &dist_r_m);

    // convert dist to count of holes
    int convert_dist_m_to_n(float dist_m);

    /*
    ######################################################################################
    # Internal variables #
    ######################################################################################
    */
    // OBSOLETE - no more internal state
    // used to cache phi angle in rad from prev update
    // float phi;

    // vehicle parameters
    float wheels_base;
    float wheels_radius;
    int odometer_holes_count;
};