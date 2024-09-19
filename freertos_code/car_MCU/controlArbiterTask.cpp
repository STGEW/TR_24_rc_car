#include "hardware/gpio.h"
#include <stdio.h>

#include "controlArbiterTask.h"
#include "uartHandlerTask.h"
#include "motorDriver.h"
#include "../utils.h"
#include "joystick.h"
#include "autonomousMode.h"

#define JOYSTICK_TIMEOUT_SEC                1.0
#define VEH_UPD_POS_MSG_PER_SEC             0.2


void dataEnginesToDriver(EnginesPwr & engines, DriverControlData & driver);


// initialize required for motor pins
void setupControlArbiterTask( void )
{
    // intially OFF
    gpio_init(RADIO_LED_PIN);
    gpio_set_dir(RADIO_LED_PIN, GPIO_OUT);
    gpio_put(RADIO_LED_PIN, 0);
}


void runControlArbiterTask( void *pvParameters )
{
    EnginesPwr engines_pwr = {0, 0};
    DriverControlData d;
    bool ext_ctrl = false;
    bool ext_ctrl_prev = false;
    bool joy_notified_once = false;

    Joystick joystick = Joystick();
    TickType_t lastJoyRxTick = xTaskGetTickCount();
    float secSinceLastJoyRx;

    Vehicle2DPosition veh_pos;
    TickType_t lastVehiclePosTick = xTaskGetTickCount();
    float secSinceLastVehiclePos;

    Point2D point_2D; // new task for path planning
    AutonomousMode autoMode = AutonomousMode();

    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();


    for( ;; )
    {
        if (true == joystick.receive(engines_pwr, ext_ctrl)) {
            joy_notified_once = false;
            // printf("Received data from joystick\n");
            gpio_put(RADIO_LED_PIN, 1);
            // update timings
            lastJoyRxTick = xTaskGetTickCount();

            if (true == ext_ctrl) {
                // ext control
                if (true == check_new_point_parsed(point_2D)) {
                    autoMode.init(point_2D);
                } else if (true == stop_cmd_received()) {
                    autoMode.reset();
                } else {
                    autoMode.update();
                }

                if (true == autoMode.is_done()) {
                    send_done();
                }

                // send vehicles position to ext PC
                // only if we are executing a task right now
                if (    (autoMode.read_state() != WAITING_FOR_TASK) &&
                        (autoMode.read_state() != DONE)) {
                    secSinceLastVehiclePos = secondsSinceLastTick(lastVehiclePosTick);
                    if (secSinceLastVehiclePos > VEH_UPD_POS_MSG_PER_SEC) {
                        lastVehiclePosTick = xTaskGetTickCount();
                        autoMode.get_vehicle_pos(veh_pos);
                        send_position(veh_pos);
                    }
                }

            } else {
                // manual ctrl
                if ((true == ext_ctrl_prev) && (false == ext_ctrl)) {
                    // when the switching between auto control
                    // and ext control happened, we should
                    // notify ext PC
                    send_abort();
                    // reset the state of automode
                    autoMode.reset();
                }

                // joystick has first priotity
                dataEnginesToDriver(engines_pwr, d);
                runMotorDriver(d);
            }
            ext_ctrl_prev = ext_ctrl;
        } else {
            secSinceLastJoyRx = secondsSinceLastTick(lastJoyRxTick);

            if ((secSinceLastJoyRx > JOYSTICK_TIMEOUT_SEC) &&
                    (false == joy_notified_once)) {
                joy_notified_once = true;
                // no data from joystick since reasonable amount of time
                gpio_put(RADIO_LED_PIN, 0);
                // stopping the car
                d.direction_A = OFF; d.direction_B = OFF;
                d.duty_cycle_A = 0; d.duty_cycle_B = 0;
                runMotorDriver(d);

                send_abort();
                autoMode.reset();
            }
        }

        // 10 msec delay ~ 100 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}

void dataEnginesToDriver(EnginesPwr & engines, DriverControlData & d) {
    auto helper = [](int32_t e, int & dir, int & duty) -> void {
        if (e == 0) {
            dir = OFF;
            duty = 0;
        } else if (e > 0) {
            dir = FORWARD;
            duty = e * DRIVER_COEF;
        } else if (e < 0) {
            dir = REVERSE;
            duty = -1 * e * DRIVER_COEF;
        }
    };

    helper(engines.left, d.direction_A, d.duty_cycle_A);
    helper(engines.right, d.direction_B, d.duty_cycle_B);
}
