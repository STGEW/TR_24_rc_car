#include "pid_controller.h"


PIDController::PIDController(float _Kp, float _Ki, float _Kd) {
    Kp = _Kp;
    Ki = _Ki;
    Kd = _Kd;
};

void PIDController::restart(float _t_prev) {
    t_prev = _t_prev;
    t = _t_prev;
    
    eD_prev = 0.0f;
    eD = 0.0f;

    I = 0.0f;
};

float PIDController::update(float _t, float _PV, float _SP) {
    t = _t; PV = _PV; SP = _SP;
    float P, D, MV;

    P = Kp * (beta * SP - PV);
    I = I + Ki * (SP - PV) * (t - t_prev);
    eD = gamma * SP - PV;
    D = Kd * (eD - eD_prev) / (t - t_prev);
    MV = MV_bar + P + I + D;
    eD_prev = eD;
    t_prev = t;

    return MV;
}
