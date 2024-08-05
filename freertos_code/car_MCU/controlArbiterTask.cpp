#include "hardware/gpio.h"
#include <stdio.h>

#include "controlArbiterTask.h"
#include "sensorFusionTask.h"
#include "motorDriver.h"
#include "controller.h"
#include "../utils.h"


SemaphoreHandle_t inputResolverRfDataMutex;
SemaphoreHandle_t inputResolverPathPlanningMutex;

bool ext_control = false;
struct EnginesPwr rf_engines_pwr = {0, 0};
TickType_t lastRFDataTick = 0.0;

PathPlanningData inp_res_pp_data = {0,0,0,0};
bool pp_updated = false;


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
    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    TickType_t lastCmdTicks = xTaskGetTickCount();
    TickType_t sinceLastCmdTicks = 0;
    float sinceLastCmdSec = 0.0;

    bool _ext_control;
    struct EnginesPwr _engines_pwr;
    TickType_t _lastRFDataTick = xTaskGetTickCount();

    PathPlanningData _path_planning_data;
    bool _pp_updated;

    float secSinceLastRFData;

    DriverControlData driver;
    SensorsData sensors_data;

    Controller controller = Controller();

    for( ;; )
    {
        // non blocking!
        if (xSemaphoreTake(inputResolverRfDataMutex, 0) == pdTRUE) {
            // copying the state of global variables to local variables
            _ext_control = ext_control;
            _engines_pwr = rf_engines_pwr;
            _lastRFDataTick = lastRFDataTick;
            xSemaphoreGive(inputResolverRfDataMutex);
        }

        // Do we need it????
        // non blocking!
        if (xSemaphoreTake(inputResolverPathPlanningMutex, 0) == pdTRUE) {
            // copying the state of global variables to local variables
            _pp_updated = pp_updated;
            _path_planning_data = inp_res_pp_data;
            xSemaphoreGive(inputResolverPathPlanningMutex);
        }

        secSinceLastRFData = secondsSinceLastTick(_lastRFDataTick);
        
        if (secSinceLastRFData > JOYSTICK_TIMEOUT_SEC) {
            // no data from joystick since reasonable amount of time
            gpio_put(RADIO_LED_PIN, 0); // LED indication
            // stopping the car
            driver.direction_A = OFF; driver.direction_B = OFF;
        } else {
            // fresh data from joystick
            gpio_put(RADIO_LED_PIN, 1); // LED indication
            if (_ext_control) {
                printf("ext control TRUE branch\n");
                if ( true == _pp_updated ) {
                    // ?
                    controller.start(_path_planning_data);
                    reset_sensor_fusion_task();
                } else {
                    read_fused_sensors_data(sensors_data);
                    // ?
                    controller.update(_engines_pwr, sensors_data);
                    dataEnginesToDriver(_engines_pwr, driver);
                }
            } else {
                printf("ext control FALSE branch\n");
                //somehow synchronize with driver task
                dataEnginesToDriver(_engines_pwr, driver);
            }
        }
        runMotorDriver(driver);

        // 10 msec delay ~ 100 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}

void dataEnginesToDriver(EnginesPwr & engines, DriverControlData & driver) {
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

    helper(engines.left, driver.direction_A, driver.duty_cycle_A);
    helper(engines.right, driver.direction_B, driver.duty_cycle_B);
}
