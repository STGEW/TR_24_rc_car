#include <math.h>
#include <stdio.h>

#include "differentialVehicleModel.h"


// wheel_base - a distance between left and right wheels in meters, f.e. 0.15 m
// wheels_radius - a radius of wheel in meters, f.e. 0.065 m
// odometer_holes_count - total count of holes in a wheel, f.e. 20
DifferentialVehicleModel::DifferentialVehicleModel(float _wheels_base,
        float _wheels_radius, int _odometer_holes_count) {
    wheels_base = _wheels_base;
    wheels_radius = _wheels_radius;
    odometer_holes_count = _odometer_holes_count;
}


// no need to keep state
// dist - meters, float
int DifferentialVehicleModel::dist_diff_to_odo(float d_dist) {
    return convert_dist_m_to_n(d_dist);
}

// returns a odo for right wheel.
// positive d_yar means position right wheel odo value
// left odo value = -1 * right odo value
int DifferentialVehicleModel::yaw_diff_to_odo(float d_yaw) {
    // 1. yaw diff to dist
    float d_dist = d_yaw * (wheels_base / 2.0);
    // 2. dist to odo
    return convert_dist_m_to_n(d_dist);

}

// delta_n is expected position
// direction information in clockwise flag
void DifferentialVehicleModel::odo_to_pos_rotate(
        int delta_n, bool clockwise,
        Vehicle2DPosition &d_pos) {
    d_pos.x = 0.0f; d_pos.y = 0.0f;
    // a dist that wheel will move
    float d_dist = ((float) delta_n / (float) odometer_holes_count) * 2 * M_PI * wheels_radius;
    // we are rotation around the center of vehicle
    d_pos.phi = d_dist / (wheels_base / 2.0);

    if (true == clockwise) {
        // negative direction
        d_pos.phi = -1.0f * d_pos.phi;
    }
}

// we expect delta_n - position and direction info inside
// forward flag
void DifferentialVehicleModel::odo_to_pos_linear(
        int delta_n, bool forward,
        float phi, Vehicle2DPosition &d_pos) {
    float d_dist = ((float) delta_n / (float) odometer_holes_count) * 2 * M_PI * wheels_radius;
    d_pos.x = d_dist * cos(phi);
    d_pos.y = d_dist * sin(phi);
    if (false == forward) {
        d_pos.x = -1.0f * d_pos.x;
        d_pos.y = -1.0f * d_pos.y;
    }
}


// OBSOLETE
// Calculate the state of vehicle
// int delta_n_r - count of measured holes from the right odometer
// int delta_n_l - count of measured holes from the left odometer
// Vehicle2DPosition - x,y,phi
// void DifferentialVehicleModel::odometer_to_position_diff(
//         int delta_n_r, int delta_n_l,
//         Vehicle2DPosition &position_diff) {

    // printf(
    //     "wheels_base: %f wheels_radius: %f odometer_holes_count: %d\n",
    //     wheels_base, wheels_radius, odometer_holes_count);

    // printf("update called with values: %d %d\n", delta_n_r, delta_n_l);
    // float delta_d_r = calc_delta_d(delta_n_r);
    // float delta_d_l = calc_delta_d(delta_n_l);

    // printf("delta r: %f l: %f\n", delta_d_r, delta_d_l);
    // float turn_radius = calc_turning_radius(
    //     delta_d_r, delta_d_l);

    // printf("turn_radius: %f\n", turn_radius);
    // float delta_phi = calc_delta_phi(
    //     delta_d_r, delta_d_l, turn_radius);

    // printf("delta_phi: %f\n", delta_phi);
    // float phi_prev = phi;
    // phi += delta_phi;
    // printf("phi: %f\n", phi);
    // position_diff.x = calc_x(delta_d_r, delta_d_l, phi_prev, phi, turn_radius);
    // position_diff.y = calc_y(delta_d_r, delta_d_l, phi_prev, phi, turn_radius);
    // position_diff.phi = delta_phi;
    // printf("delta x: %f, delta y: %f delta phi: %f\n",
    //     position_diff.x, position_diff.y, position_diff.phi);
// }


// Convert a path chunk x1,y1,phi1,x2,y2 to expected from odometers values
void DifferentialVehicleModel::path_to_odometer_values(
        PathChunk &path,
        OdometerValues &odo) {

    printf("path_to_odometer_values\n");
    // step 1 - calculate new phi angle
    odo.phi2 = atan2(
        path.y2 - path.y1,
        path.x2 - path.x1);

    printf("phi2: %f\n", odo.phi2);
    float d_yaw = odo.phi2 - path.phi1;
    while (d_yaw > M_PI) d_yaw -= 2.0 * M_PI;
    while (d_yaw < -M_PI) d_yaw += 2.0 * M_PI;

    printf("d_yaw: %f\n", d_yaw);
    if (fabs(d_yaw) < 0.005f) {
        // no yaw change
        printf("No yaw change\n");
        odo.angle_n_r = 0;
        odo.angle_n_l = 0;
    } else {
        // yaw change
        printf("Yaw changed\n");
        odo.angle_n_r = yaw_diff_to_odo(d_yaw);
        odo.angle_n_l = -1 * odo.angle_n_r;
        printf("odo angle r: %d l: %d\n", odo.angle_n_r, odo.angle_n_l);
    }

    float d_dist = sqrt(
        square(path.x2 - path.x1) +
        square(path.y2 - path.y1));
    printf("odo d_dist: %f\n", d_dist);

    if (fabs(d_dist) < 0.005f) {
        // no pos change
        odo.dist_n = 0.0f;
    } else {
        odo.dist_n = dist_diff_to_odo(d_dist);
        printf("odo n count: %d\n", odo.dist_n);
    }


    // prev way
    // odo.phi2 = atan2(
    //     path.y2 - path.y1,
    //     path.x2 - path.x1);
    // printf("Phi 2: %f\n", odo.phi2);
    // float dist_m = sqrt(
    //     square(path.x2 - path.x1) +
    //     square(path.y2 - path.y1));
    // float delta_phi = odo.phi2 - path.phi1;
    // // normalizing angles
    // while (delta_phi > M_PI) delta_phi -= 2.0 * M_PI;
    // while (delta_phi < -M_PI) delta_phi += 2.0 * M_PI;
    // printf("Delta phi: %f\n", delta_phi);

    // // step 1 - change phi angle
    // convert_delta_phi_to_n(delta_phi,
    //     odo.angle_n_l,
    //     odo.angle_n_r);

    // // step 2 - change dist
    // odo.dist_n = convert_dist_m_to_n(dist_m);
}

// OBSOLETE - no more internal state
// reset internal information of the model
// void DifferentialVehicleModel::reset_internal_state() {
//     phi = 0.0f;
// }

/*
######################################################################################
# Methods which are used to calculate from odometer's data a position of the vehicle #
# odometer_to_position_diff - is an interface                                        #
######################################################################################
*/
// OBSOLETE
// delta_n - count of holes measured by odometer
// return delta_d (float) - the length of the path in meters
// float DifferentialVehicleModel::calc_delta_d(int delta_n) {
//     return ((float) delta_n) * 2.0 * M_PI * wheels_radius / odometer_holes_count;
// }

// OBSOLETE
// turning radius. can be called only when delta_d_r != delta_d_l
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// return turning_radius (meters)
// float DifferentialVehicleModel::calc_turning_radius(float delta_d_r, float delta_d_l) {
//     if (delta_d_l == delta_d_r) {
//         return INFINITY;
//     }
//     return delta_d_r * wheels_base / (delta_d_l - delta_d_r);
// }

// OBSOLETE
// calculate a delta phi for the movement in radians
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// turn_radius (float) - meters, a turning radius   
// return delta_phi (float) - rad, a diff in angle for delta 
// float DifferentialVehicleModel::calc_delta_phi(float delta_d_r, float delta_d_l, float turn_radius) {
//     if (delta_d_l == delta_d_r) {
//         return 0.0f;
//     }
//     // delta_d_l != delta_d_r
//     if (turn_radius == 0.0f) {
//         return delta_d_r / (wheels_base);
//     } else if (turn_radius < 0.0f){
//         return (-1.0) * delta_d_r / turn_radius;
//     } else {
//         // turn_radius > 0.0
//         return delta_d_l / turn_radius;
//     }
// }

// OBSOLETE
// at i interval
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// phi_prev (float) - radians, at i-1 of an angle
// phi (float) - radians, at i of an angle
// turn_radius (float) - meters, a turning radius
// return x at interval i (float)
// float DifferentialVehicleModel::calc_x(
//         float delta_d_r, float delta_d_l,
//         float phi_prev, float phi,
//         float turn_radius) {
//     if (INFINITY == turn_radius) {
//         return cos(phi) * (delta_d_r + delta_d_l) / 2.0f;
//     } else if (0.0f == turn_radius) {
//         return 0.0f;
//     }
//     // I have doubts in that equation, but we don't use that case
//     // return (turn_radius + wheels_base / 2.0) * (
//     //     sin(phi_prev + M_PI / 2.0f) + sin(phi - M_PI / 2.0f));
//     return turn_radius * (sin(phi) - sin(phi_prev));
// }

// OBSOLETE
// at i interval
// delta_d_r (float) - meters of movement right side
// delta_d_l (float) - meters of movement left side
// phi_prev (float) - radians, at i-1 of an angle
// phi (float) - radians, at i of an angle
// turn_radius (float) - meters, a turning radius
// return y at interval i (float)
// float DifferentialVehicleModel::calc_y(
//         float delta_d_r, float delta_d_l,
//         float phi_prev, float phi,
//         float turn_radius) {
//     if (INFINITY == turn_radius) {
//         return sin(phi) * (delta_d_r + delta_d_l) / 2.0f;
//     } else if (0.0f == turn_radius) {
//         return 0.0f;
//     }
//     // I have doubts in that equation, but we don't use that case
//     // return (turn_radius + wheels_base / 2.0f) * (
//     //         cos(phi_prev + M_PI / 2.0f) + cos(phi - M_PI / 2.0f));
//     return turn_radius * (cos(phi_prev) - cos(phi));
// }

/*
######################################################################################
# Methods which are used to convert a path chunk into expected from odometer values  #
# path_to_odometer_values - is an interface                                          #
######################################################################################
*/
float DifferentialVehicleModel::square(float x) {
    return x * x;
}

// OBSOLETE
// void DifferentialVehicleModel::convert_delta_phi_to_n(
//         float delta_phi, int &angle_n_l, int &angle_n_r) {
//     float dist_l_m, dist_r_m;
//     convert_delta_phi_to_dist_m(delta_phi, dist_l_m, dist_r_m);
//     angle_n_l = convert_dist_m_to_n(dist_l_m);
//     // we can call a function second time.
//     // but it's obvious that the only difference is opposite sign
//     angle_n_r = - angle_n_l;
// }

/*
    step 1
    convert phi into the distance (meters)
    2 * pi * (wheels_base / 2.0) ~ 2 * pi
    x (a dist in meters) ~ phi
    x =  phi * (wheels_base / 2.0) <- dist m
*/
// OBSOLETE
// void DifferentialVehicleModel::convert_delta_phi_to_dist_m(
//         float delta_phi, float &dist_l_m, float &dist_r_m) {
//     float dist_m = fabs(delta_phi) * (wheels_base);
//     // printf("For delta phi: %f dist: %3f\n", delta_phi, dist_m);

//     if (delta_phi < 0) {
//         dist_l_m = dist_m;
//         dist_r_m = -dist_m;
//     } else {
//         dist_l_m = -dist_m;
//         dist_r_m = dist_m;
//     }
// }

/*
    step 2
    convert dist in meters to n (count of holes for the odometer)
    2 * pi * wheel_radius ~ max_n
    dist ~ n
    n = (dist * max_n) / (2 * pi * wheel_radius)
*/

int DifferentialVehicleModel::convert_dist_m_to_n(float dist_m) {
    float n = (dist_m * odometer_holes_count) / (2.0f * M_PI * wheels_radius);
    // printf("For dist_m: %f n: %f\n", dist_m, n);
    int rounded_n = round(n);
    // printf("Rounding n: %d\n", rounded_n);
    return rounded_n;
}
