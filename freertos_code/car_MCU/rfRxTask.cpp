#include <stdio.h>
#include <RF24.h>         // RF24 radio object
#include "rfRxTask.h"
#include "commandsProcessorTask.h"

// instantiate an object for the nRF24L01 transceiver
RF24 radio(CE_PIN, CSN_PIN);
// float payload = 0.0;
// Used to control whether this node is sending or receiving
bool role = false; // true = TX role, false = RX role

void prvSetupRFHardware()
{
    uint8_t address[][6] = {"1Node", "2Node"};
    bool radioNumber = 1;

    // initialize the transceiver on the SPI bus
    if (!radio.begin()) {
        printf("radio hardware is not responding!!\n");
        return;
    }
    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(joystick)); // float datatype occupies 4 bytes
    // set the TX address of the RX node into the TX pipe
    radio.openWritingPipe(address[radioNumber]); // always uses pipe 0
    // set the RX address of the TX node into a RX pipe
    radio.openReadingPipe(1, address[!radioNumber]); // using pipe 1
    radio.setChannel(124);
    radio.setDataRate(RF24_2MBPS);
    radio.setCRCLength(RF24_CRC_16);
    if (role) {
        radio.stopListening(); // put radio in TX mode
    }
    else {
        radio.startListening(); // put radio in RX mode
    }

}

void rfRxTask( void *pvParameters )
{

    TickType_t xNextWakeTime;
    const unsigned long ulValueToSend = 100UL;

    /* Remove compiler warning about unused parameter. */
    ( void ) pvParameters;

    /* Initialise xNextWakeTime - this only needs to be done once. */
    xNextWakeTime = xTaskGetTickCount();

    for( ;; )
    {
        // This device is a RX node
        uint8_t pipe;
        if (radio.available()) {
             // is there a payload? get the pipe number that recieved it
            uint8_t bytes = radio.getPayloadSize(); // get the size of the payload
            radio.read(&joystick, bytes);            // fetch payload from FIFO
            // print the size of the payload, the pipe number, payload's value
            printf("Received %d bytes: %u %u\n", bytes, joystick.x, joystick.y);
            xSemaphoreGive(commandsProcessorSemaphore);
        } else {
            // printf("RADIO IS NOT AVAIL\n");
        }

        // 10 msec delay ~ 100 Hz
        vTaskDelay(pdMS_TO_TICKS( 10 ));
    }
}
