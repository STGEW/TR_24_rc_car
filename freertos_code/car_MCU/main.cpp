/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "TimerDemo.h"
#include "QueueOverwrite.h"
#include "EventGroupsDemo.h"
#include "IntSemTest.h"
#include "TaskNotify.h"

#include "motorDriverTask.h"
#include "inputResolverTask.h"
#include "cameraServoTask.h"
#include "hardware/gpio.h"

#include "../const.h"

/* Library includes. */
#include <stdio.h>
#include "pico/stdlib.h"
#if ( mainRUN_ON_CORE == 1 )
#include "pico/multicore.h"
#endif

/*-----------------------------------------------------------*/

// rfRXTask - responsible for receiving data from radio
extern void setupRfRxTask( void );
extern void runRfRxTask( void *pvParameters );

// motorDriverTask functions
extern void setupMotorDriverTask( void );
extern void runMotorDriverTask( void *pvParameters );

// inputResolverTask functions
extern void setupInputResolverTask( void );
extern void runInputResolverTask( void *pvParameters );

// imuRawTask functions
extern void setupIMURawTask();
extern void runIMURawTask( void *pvParameters );

// cameraServoControlTask functions
extern void setupCameraServoTask();
extern void runCameraServoTask( void *pvParameters );

// uartHandlerTask functions
extern void setupUartRxHardware();
extern void runUartRxTask( void *pvParameters );

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

    motorDriverSemaphore = xSemaphoreCreateBinary();
    inputResolverMutex = xSemaphoreCreateMutex();
    cameraServoSemaphore = xSemaphoreCreateBinary();

    xTaskCreate( runMotorDriverTask, "motorDriverTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    xTaskCreate( runInputResolverTask, "runInputResolverTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate( runRfRxTask, "rfRxTask",                     
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate( runIMURawTask, "imuRawTask",
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate( runCameraServoTask, "cameraServoTask",                     
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

    xTaskCreate( runUartRxTask, "runUartRxTask",                     
        configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL );

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
    setupMotorDriverTask();
    setupRfRxTask();
    setupIMURawTask();
    setupInputResolverTask();
    setupUartRxHardware();
    setupCameraServoTask();

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