#include "rfTxTask.h"
#include <stdio.h>
#include <RF24.h>         // RF24 radio object
#include <tusb.h>         // tud_cdc_connected()

// semaphore for reading data by commandsProcessor
SemaphoreHandle_t rfTxSemaphore;
struct JoystickRawData joystick = {0, 0};

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
bool role = true; // true = TX role, false = RX role


void prvSetupRFHardware()
{
    // Let these addresses be used for the pair
    uint8_t address[][6] = {"1Node", "2Node"};
    bool radioNumber = 0;

    // wait here until the CDC ACM (serial port emulation) is connected
    // while (!tud_cdc_connected()) {
    //     sleep_ms(10);
    // }

    // initialize the transceiver on the SPI bus
    if (!radio.begin()) {
        printf("radio hardware is not responding!!\n");
        return;
    }
    radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.
    radio.setPayloadSize(sizeof(joystick)); // float datatype occupies 4 bytes
    radio.openWritingPipe(address[radioNumber]);
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1
    radio.setChannel(124);
    radio.setDataRate(RF24_2MBPS);
    // radio.enableDynamicPayloads();
    // radio.startListening();
    radio.setCRCLength(RF24_CRC_16);
    if (role) {
        radio.stopListening(); // put radio in TX mode
    }
    else {
        radio.startListening(); // put radio in RX mode
    }
}

void rfTxTask( void *pvParameters )
{

    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {

        if (xSemaphoreTake(rfTxSemaphore, 0) == pdTRUE) {
            printf(
                "joystick's values for processing are. x: %u, y: %u\n",
                joystick.x,
                joystick.y);

            // radio.stopListening();
            // This device is a TX node
            bool report = radio.write(&joystick, sizeof(joystick));         // transmit & save the report
            // radio.stopListening();
            if (report) {
                // payload was delivered; print the payload sent & the timer result
                printf("Transmission successful!\n");
            }
            else {
                // payload was not delivered
                printf("Transmission failed or timed out\n");
            }

            xSemaphoreGive(rfTxSemaphore);

       } else {
           printf("RFTXTask didn't get a semaphore\n");
       }

        // 100 msec delay ~ 10 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
