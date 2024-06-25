#pragma once

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

#define ADC_CHANNEL_X       0
#define ADC_CHANNEL_Y       1

#define UART_CTRL_ID        uart1
#define UART_CTRL_BAUD_RATE 460800

// UART PARSER
#define UART_CMD_HEADER_SIZE  2
#define UART_CMD_HEADER       "SV"
#define UART_CMD_SIZE_BYTES   8

// JOYSTICK
// contains raw measurements from joystick
struct __attribute__((__packed__)) JoystickRawData {
    uint x;
    uint y;
    bool ext_control;
};

// BUTTON STATES
#define BUTTON_DN 0
#define BUTTON_UP 1

// Conversion from joystick to engine state
/*
What we get from joystick?
0..4095 as a full range.

Regarding joystick position:
0..2047 - left
2048..4095 - right

How we interpret these values?
2048 - DEAD_ZONE ... 2048 + DEAD_ZONE - safe zone / dead zone / neutral

0 ... 2048 - DEAD_ZONE - 'backward'/'left'
2048 + DEAD_ZONE ... 4095 - 'forward'/'right'
*/
#define DEAD_ZONE               50
#define DEAD_ZONE_START         2048 - DEAD_ZONE
#define DEAD_ZONE_END           2048 + DEAD_ZONE
#define TURN_COEF               0.60    // this coefficient is chosen after several experiments
#define ADC_TO_PERCENTAGE       100.0 / 2048.0
#define JOYSTICK_TIMEOUT_SEC    1.0


// Engine Power state
struct __attribute__((__packed__)) EnginesPwr {
    int32_t left;
    int32_t right;
};
