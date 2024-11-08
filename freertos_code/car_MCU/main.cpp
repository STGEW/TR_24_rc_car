/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "TimerDemo.h"
#include "QueueOverwrite.h"
#include "EventGroupsDemo.h"
#include "IntSemTest.h"
#include "TaskNotify.h"

#include "controlArbiterTask.h"
#include "cameraServoTask.h"
#include "hardware/gpio.h"
#include "odometerTask.h"
#include "uartHandlerTask.h"
#include "imuRawTask.h"


#include "../const.h"

/* Library includes. */
#include <stdio.h>
#include "pico/stdlib.h"
#if ( mainRUN_ON_CORE == 1 )
#include "pico/multicore.h"
#endif

/*-----------------------------------------------------------*/
// motorDriverTask functions
extern void setupMotorDriver( void );

// controlArbiterTask functions
extern void setupControlArbiterTask( void );
extern void runControlArbiterTask( void *pvParameters );

// imuRawTask functions
extern void setupIMURawTask();
extern void runIMURawTask( void *pvParameters );

// cameraServoControlTask functions
extern void setupCameraServoTask();
extern void runCameraServoTask( void *pvParameters );

// uartHandlerTask functions
extern void setupUartHandlerHardware();
extern void runUartHandlerTask(void *pvParameters);

// odometerTask functions
extern TaskHandle_t xOdometerLeftTaskHandle;
extern TaskHandle_t xOdometerRightTaskHandle;
extern void setupOdometerTask();
extern void runOdometerLeftTask(void *pvParameters );
extern void runOdometerRightTask(void *pvParameters );

static void prvSetupHardware( void );

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file. */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );
/*-----------------------------------------------------------*/

void core_1_tasks( void )
{

    cameraServoSemaphore = xSemaphoreCreateBinary();
    odometerMutex = xSemaphoreCreateMutex();
    imuMutex = xSemaphoreCreateMutex();
    uartStopCMDMutex = xSemaphoreCreateMutex();

    // Working with UART queues
    // To send something to UART
    q_uart_tx = xQueueCreate(10, sizeof(char*));
    if (NULL == q_uart_tx) {
        printf("ERROR! q_uart_tx can't be created\n");
        return;
    }

    // To read points from UART
    q_target_point_2D = xQueueCreate(10, sizeof(Point2D));
    if (NULL == q_target_point_2D) {
        printf("ERROR! q_target_point_2D can't be created\n");
        return;
    }

    // these are positions (x,y,phi) of the car
    q_car_pos = xQueueCreate(20, sizeof(Vehicle2DPosition));
    if (NULL == q_car_pos) {
        printf("ERROR! q_car_pos can't be created\n");
        return;
    }    

    xTaskCreate( runControlArbiterTask, "runControlArbiterTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate( runIMURawTask, "imuRawTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    // xTaskCreate( runCameraServoTask, "cameraServoTask",                     
    //     configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate( runUartHandlerTask, "runUartHandlerTask",                     
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate(runOdometerLeftTask, "runOdometerLeftTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1,
        &xOdometerLeftTaskHandle);

    xTaskCreate(runOdometerRightTask, "runOdometerRightTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1,
        &xOdometerRightTaskHandle);

    vTaskStartScheduler();

    for( ;; );
}

int main( void )
{
    /* Configure the hardware ready to run the demo. */
    prvSetupHardware();
    const char *rtos_name = "FreeRTOS SMP";
    core_1_tasks();
    // Launch FreeRTOS on core 0 and core 1
    // multicore_launch_core1(core_1_tasks);
    return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
    stdio_init_all();
    // By default uart 0 has BR 115200, we want it to be higher
    uart_init(UART_LOG_ID, UART_LOG_BAUD_RATE);
    gpio_set_function(UART_LOG_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_LOG_RX_PIN, GPIO_FUNC_UART);
    uart_puts(uart0, "RC car is ready\n");

    setupMotorDriver();
    setupIMURawTask();
    setupControlArbiterTask();
    setupUartHandlerHardware();
    setupCameraServoTask();
    setupOdometerTask();

    gpio_init(BOOT_LED_PIN);
    gpio_set_dir(BOOT_LED_PIN, GPIO_OUT);
    gpio_put(BOOT_LED_PIN, 1);
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
    /* Called if a call to pvPortMalloc() fails because there is insufficient
    free memory available in the FreeRTOS heap.  pvPortMalloc() is called
    internally by FreeRTOS API functions that create tasks, queues, software
    timers, and semaphores.  The size of the FreeRTOS heap is set by the
    configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */

    /* Force an assert. */
    configASSERT( ( volatile void * ) NULL );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    volatile size_t xFreeHeapSpace;

    /* This is just a trivial example of an idle hook.  It is called on each
    cycle of the idle task.  It must *NOT* attempt to block.  In this case the
    idle task just queries the amount of FreeRTOS heap that remains.  See the
    memory management section on the http://www.FreeRTOS.org web site for memory
    management options.  If there is a lot of heap memory free then the
    configTOTAL_HEAP_SIZE value in FreeRTOSConfig.h can be reduced to free up
    RAM. */
    xFreeHeapSpace = xPortGetFreeHeapSize();

    /* Remove compiler warning about xFreeHeapSpace being set but never used. */
    ( void ) xFreeHeapSpace;
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
}