#include "uartHandlerTask.h"
#include "DataParser.h"


void runParserTask(void *pvParameters) {
    bool parsed = false;
    PathPlanningData path_planning_data = PathPlanningData{0,0,0,0};
    DataParser parser = DataParser();

    for( ;; )
    {
        // Reading from UART, non-blocking
        if (xSemaphoreTake(uartMutex, portMAX_DELAY) == pdTRUE) {
            for (int i = 0; i < uart_buffer_counter; i++) {
                parsed = parser(uart_buffer[i], &path_planning_data);
            }
            xSemaphoreGive(uartMutex);
        }
        // wait for the semaphone and send to InputResolverTask
        if (true == parsed) {
            if (xSemaphoreTake(inputResolverPathPlanningMutex, portMAX_DELAY) == pdTRUE) {
                inp_res_pp_data = path_planning_data;
                pp_update = true;
                xSemaphoreGive(inputResolverPathPlanningMutex);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}