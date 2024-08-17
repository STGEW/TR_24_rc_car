#include "joystick.h"


Joystick::Joystick() {
    uint8_t address[][6] = {"1Node", "2Node"};
    bool radioNumber = 1;

    // initialize the transceiver on the SPI bus
    if (!radio.begin()) {
        printf("radio hardware is not responding!!\n");
        return;
    }
    radio.setPALevel(RF24_PA_LOW);
    radio.setPayloadSize(sizeof(JoystickRawData)); // float datatype occupies 4 bytes
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

bool Joystick::receive(EnginesPwr &engines_pwr, bool &ext_ctrl)
{
    bool is_avail = radio.available();
    if (true == is_avail) {
        uint8_t bytes = radio.getPayloadSize();
        radio.read(&joystick, bytes);
        // printf("RF Received %d bytes: %u %u %s\n",
        //     bytes,
        //     joystick.x, joystick.y,
        //     joystick.ext_control ? "true" : "false");
        convToEnginesPwrs(joystick, engines_pwr);
        ext_ctrl = joystick.ext_control;
    }
    return is_avail;
}

void Joystick::convToEnginesPwrs(JoystickRawData & joy, EnginesPwr & pwr) {

    int turn = 0;
    int forward = 0;
    
    // printf("Step 1 - processing 'dead zones'\n");
    if ((joy.x >= DEAD_ZONE_START) && (joy.x <= DEAD_ZONE_END)) {
        forward = 0;
    } else {
        forward = joy.x;
        forward -= 2048; // centering
    }

    if ((joy.y >= DEAD_ZONE_START) && (joy.y <= DEAD_ZONE_END)) {
        turn = 0;
    } else {
        turn = joy.y;
        turn -= 2048; // centering
    }
    
    // printf(
    //     "After excluding 'dead zones' and centering around 0; forward: %d, turn: %d\n",
    //     forward, turn);
    

    // printf("Step 2 - convert values to the range -100%...+100%\n");
    forward = forward * ADC_TO_PERCENTAGE;
    turn = turn * ADC_TO_PERCENTAGE;
    // printf(
    //     "After conversion to -100%...+100% range; forward: %d, turn: %d\n",
    //     forward, turn);

    // printf("Step 3 - convert jostick's values to power lvls for left and right sides\n");

    // first we apply forward/backward:
    pwr.left = forward;
    pwr.right = forward;
    // printf(
    //     "After applying forward values; left_side_power: %d right_side_power: %d\n",
    //     left_side_power,
    //     right_side_power);

    if (turn > 0) {
        // turn to the right. Left wheels should rotate faster
        pwr.left += turn * TURN_COEF;
        pwr.right -= turn * TURN_COEF;
    } else if (turn < 0) {
        // turn to the left. Right wheels should rotate faster than left
        pwr.left -= -1 * turn * TURN_COEF;
        pwr.right += -1 * turn * TURN_COEF;
    }
    // printf(
    //     "After applying turn values; left_side_power: %d right_side_power: %d\n",
    //     left_side_power,
    //     right_side_power);

    // limit within -100..100 range 
    if (pwr.left > 100) {
        pwr.left = 100;
    } else if (pwr.left < -100) {
        pwr.left = -100;
    }

    if (pwr.right > 100) {
        pwr.right = 100;
    } else if (pwr.right < -100) {
        pwr.right = -100;
    } 
    // printf(
    //     "After limiting within -100..100; left_side_power: %d right_side_power: %d\n",
    //     left_side_power,
    //     right_side_power);
}