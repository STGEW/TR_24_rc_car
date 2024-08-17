#pragma once

#include <stdint.h>

#define mainRUN_ON_CORE 1

#define IMU_I2C_SDA_PIN     2
#define IMU_I2C_SCL_PIN     3
#define UART_CTRL_TX_PIN    4
#define UART_CTRL_RX_PIN    5
#define AI1_PIN             6
#define AI2_PIN             7
#define BI1_PIN             8
#define BI2_PIN             9
#define STBY_PIN            10
#define PWM_A_PIN           11
#define PWM_B_PIN           12
#define RADIO_LED_PIN       13
#define CAMERA_SERVO_PIN    14
#define JOY_EXT_CTRL_PIN    14
#define BOOT_LED_PIN        15
#define CE_PIN              20
#define CSN_PIN             21
#define X_AXIS_PIN          26
#define Y_AXIS_PIN          27
#define ODOMETER_LEFT_PIN   26
#define ODOMETER_RIGHT_PIN  27

#define ADC_CHANNEL_X       0
#define ADC_CHANNEL_Y       1

#define UART_CTRL_ID        uart1
#define UART_CTRL_BAUD_RATE 460800


// contains raw measurements from joystick
struct __attribute__((__packed__)) JoystickRawData {
    uint32_t x;
    uint32_t y;
    bool ext_control;
};

// left -100 ... 100
// right -100 ... 100
struct __attribute__((__packed__)) EnginesPwr {
    int32_t left;
    int32_t right;
};

// Driver data
// duty cycle 0 .. 65 535
struct DriverControlData {
    int direction_A;
    int direction_B;
    int duty_cycle_A;
    int duty_cycle_B;
};

// everything in meters + radians
// TBD - Do I need it?
// struct SensorsData {
//     float d_x;
//     float d_y;
//     float d_vx;
//     float d_vy;
//     float d_heading;
// };

// ODOMETER
// describes the count of holes in used for odometer disk
#define ODOMETER_COUNT_OF_HOLES_IN_DISK     40
// #define ODOMETER_COUNT_OF_HOLES_IN_DISK     20
#define WHEEL_DIAMETER_M                    0.064
#define VEHICLE_BASE_SIZE_M                 0.13

struct __attribute__((__packed__)) Point2D {
    float x;
    float y;
};
