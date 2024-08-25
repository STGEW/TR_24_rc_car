#include "autonomousMode.h"
#include "odometerTask.h"
#include "motorDriver.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


AutonomousMode::AutonomousMode() {
    reset();
}

void AutonomousMode::reset() {
    state = AutonomousModeState::WAITING_FOR_TASK;
    // there is no more need because there is no internal state
    // diff_model.reset_internal_state();
    // reset the position
    pos.x = 0.0f; pos.y = 0.0f; pos.phi = 0.0f;
    // reset odometer balance
    odo_balance_l = 0;
    odo_balance_r = 0;
    // reset current readings
    odo_cur_l = 0;
    odo_cur_r = 0;

    // flag to work with differential model
    clockwise = false;
}

void AutonomousMode::init(Point2D &p) {
    state = AutonomousModeState::PREP_TO_YAW;

    PathChunk path;

    path.x1 = pos.x;
    path.y1 = pos.y;
    path.phi1 = pos.phi;

    path.x2 = p.x;
    path.y2 = p.y;

    printf(
        "Autonomous Mode. Init called. x1: %f y1: %f phi1: %f x2: %f y2: %f\n",
        path.x1, path.y1, path.phi1, path.x2, path.y2);

    diff_model.path_to_odometer_values(
        path,
        odo_target_vals);

    // Here we did update the position blindly
    // but instead of that we want to do that dynamically
    // pos.x = path.x2;
    // pos.y = path.y2;
    // pos.phi = odo_target_vals.phi2;
    printf(
        "Autonomous Mode. New coords would be x1: %f y1: %f phi1: %f\n",
        pos.x, pos.y, pos.phi);
}

bool AutonomousMode::is_done() {
    if (state == DONE) {
        state = WAITING_FOR_TASK;
        return true;
    } else {
        return false;
    }
}

void AutonomousMode::update() {

    read_odometer_data(odo_cur_l, odo_cur_r);

    switch (state) {
        case WAITING_FOR_TASK:
            // printf("state WAITING_FOR_TASK\n");
            d.direction_A = OFF;
            d.direction_B = OFF;
            d.duty_cycle_A = 0;
            d.duty_cycle_B = 0;
            break;

        case PREP_TO_YAW:
            printf("state PREP_TO_YAW\n");
            prepare_to_yaw(odo_target_vals.angle_n_l, d.direction_A);
            odo_balance_l = abs((int)odo_target_vals.angle_n_l);
            prepare_to_yaw(odo_target_vals.angle_n_r, d.direction_B);
            odo_balance_r = abs((int) odo_target_vals.angle_n_r);

            clockwise = odo_target_vals.angle_n_l > 0;

            state = COMPENSATE_YAW;
            break;

        case COMPENSATE_YAW:
            printf("COMPENSATE_YAW. Remaining odo left: %d right: %d\n",
                odo_balance_l, odo_balance_r);

            diff_model.odo_to_pos_rotate(
                odo_cur_r, clockwise, pos_diff);

            odo_balance_l -= odo_cur_l;
            odo_balance_r -= odo_cur_r;
            
            if ((odo_balance_l <= 0) || (odo_balance_r <= 0)) {
                d.duty_cycle_A = DRIVER_MAX_VALUE;
                d.duty_cycle_B = DRIVER_MAX_VALUE;
                d.direction_A = FORWARD;
                d.direction_B = FORWARD;
                odo_balance_l = odo_target_vals.dist_n;        
                odo_balance_r = odo_target_vals.dist_n;

                state = COMPENSATE_DIST;
            } else {
                d.duty_cycle_A = DRIVER_MAX_VALUE;
                d.duty_cycle_B = DRIVER_MAX_VALUE;
            }
            break;

        case COMPENSATE_DIST:
            printf("COMPENSATE_DIST. Remaining odo left: %d right: %d\n",
                odo_balance_l, odo_balance_r);

            diff_model.odo_to_pos_linear(
                odo_cur_r, true,
                pos.phi, pos_diff);

            odo_balance_l -= odo_cur_l;
            odo_balance_r -= odo_cur_r;
            
            if ((odo_balance_l < 0) || (odo_balance_r < 0)) {
                d.duty_cycle_A = 0;
                d.duty_cycle_B = 0;
                d.direction_A = OFF;
                d.direction_B = OFF;
                state = DONE;
            } else {
                d.duty_cycle_A = DRIVER_MAX_VALUE;
                d.duty_cycle_B = DRIVER_MAX_VALUE;
            }
            break;

        case DONE:
            d.duty_cycle_A = 0;
            d.duty_cycle_B = 0;
            d.direction_A = OFF;
            d.direction_B = OFF;
            break;

        default:
            break;
    }

    pos.x += pos_diff.x; pos_diff.x = 0.0f;
    pos.y += pos_diff.y; pos_diff.y = 0.0f;
    pos.phi += pos_diff.phi; pos_diff.phi = 0.0f;
    while (pos.phi > M_PI) pos.phi -= 2.0 * M_PI;
    while (pos.phi < -M_PI) pos.phi += 2.0 * M_PI;

    printf(
        "pos x: %f y: %f phi: %f\n",
        pos.x, pos.y, pos.phi);

    runMotorDriver(d);
}

void AutonomousMode::prepare_to_yaw(int &angle_n, int &motor_dir) {
    if (angle_n == 0) {
        motor_dir = OFF;
    } else if (angle_n < 0) {
        motor_dir = REVERSE;
    } else {
        motor_dir = FORWARD;
    }
}
