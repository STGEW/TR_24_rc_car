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
    d_pos.p.x = 0.0f; d_pos.p.y = 0.0f;
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
    d_pos.p.x = d_dist * cos(phi);
    d_pos.p.y = d_dist * sin(phi);
    if (false == forward) {
        d_pos.p.x = -1.0f * d_pos.p.x;
        d_pos.p.y = -1.0f * d_pos.p.y;
    }
}


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

}

float DifferentialVehicleModel::square(float x) {
    return x * x;
}

int DifferentialVehicleModel::convert_dist_m_to_n(float dist_m) {
    float n = (dist_m * odometer_holes_count) / (2.0f * M_PI * wheels_radius);
    // printf("For dist_m: %f n: %f\n", dist_m, n);
    int rounded_n = round(n);
    // printf("Rounding n: %d\n", rounded_n);
    return rounded_n;
}
