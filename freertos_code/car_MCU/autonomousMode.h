#pragma once
#include "../const.h"
#include "differentialVehicleModel.h"


enum AutonomousModeState {
    WAITING_FOR_TASK,
    PREP_TO_YAW,
    COMPENSATE_YAW,
    COMPENSATE_DIST,
    DONE
};


class AutonomousMode {

public:
    AutonomousMode();
    void init(Point2D &p);
    void reset();
    bool is_done();
    void update();

private:

    void prepare_to_yaw(int &angle_n, int &motor_dir);

    AutonomousModeState state;
    DifferentialVehicleModel diff_model = DifferentialVehicleModel(
        VEHICLE_BASE_SIZE_M,
        WHEEL_DIAMETER_M / 2.0,
        ODOMETER_COUNT_OF_HOLES_IN_DISK
    );

    Vehicle2DPosition pos;
    Vehicle2DPosition pos_diff;

    // keep track how many odo events left
    int odo_balance_l;
    int odo_balance_r;

    // vars where we'll store reading from odometer
    uint32_t odo_cur_l;
    uint32_t odo_cur_r;
    bool clockwise;

    DriverControlData d;

    OdometerValues odo_target_vals;
};