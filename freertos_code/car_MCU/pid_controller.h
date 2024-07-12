


class PIDController {

public:
    // (2, 0.1, 2)
    PIDController(float _Kp, float _Ki, float _Kd);
    void restart(float _t_prev);

    // returns MV - manipulated value (what we should actually set)
    float update(float _t, float _PV, float _SP);

private:

    // coeff
    float Kp;
    float Ki;
    float Kd;

    float PV; // processed output (actual measurement)
    float SP; // set point (what we want to achieve)

    float MV_bar = 0.0f;
    float beta = 1.0f; // coef proportional
    float gamma = 0.0f; // coef for derivative

    // time seconds
    float t;
    float t_prev;

    float eD;
    float eD_prev;
    float I;
};
