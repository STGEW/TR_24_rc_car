#include <cstring>
#include <stdio.h>

#include "commandsParser.h"


CommandsParser::CommandsParser(
        void (*_new_path_cb_ptr)(Point2D * point_2D),
        void (*_stop_cb_ptr)(void)) {

    new_path_cb_ptr = _new_path_cb_ptr;
    stop_cb_ptr = _stop_cb_ptr;

}

void CommandsParser::reset() {
    state = HEADER;
    counter = 0;
    exp_size = 0;
    header_buff[0] = '0'; header_buff[1] = '0';
}

void CommandsParser::parse(const char symbol) { 
    
    printf(
        "parse get new symbol: %c\n", symbol);
    
    switch (state)
    {
        case HEADER:
            printf("case HEADER\n");
            header_buff[0] = header_buff[1];
            header_buff[1] = symbol;
            if (0 == compareCharArrays(header_buff, CMDS_HEADER)) {
                state = SIZE; is_cmd = true;
            } else if(0 == compareCharArrays(header_buff, PP_HEADER)) {
                state = SIZE; is_cmd = false;
            }

            if (SIZE == state) {
                // Is that a problem???
                header_buff[0] = '0'; header_buff[1] = '0';
                printf("Moving to SIZE state:\n");
            }
            printf(" One more time Moving to SIZE state:\n");
            break;
        case SIZE:
            printf("case SIZE\n");
            exp_size = (size_t) symbol;
            counter = 0;
            state = BODY;
            printf("Switch to BODY state. Expected size: %d\n", exp_size);
            break;
        case BODY:
            printf("case BODY. Current symbol: %d\n", counter);
            body_buff[counter] = symbol;
            counter++;
            if (counter == exp_size) {
                memcpy(&point_2D, body_buff, exp_size);
                state = HEADER;
                counter = 0;
                true == is_cmd ? stop_cb_ptr() : new_path_cb_ptr(&point_2D);
            }
            break;
        default:
            break;
    }
    printf("Leaving CommandsParsers' parse function\n");
}

int CommandsParser::compareCharArrays(const char arr1[2], const char arr2[2]) {
    for (int i = 0; i < 2; ++i) {
        if (arr1[i] != arr2[i]) {
            return (arr1[i] < arr2[i]) ? -1 : 1;
        }
    }
    return 0; // Arrays are equal
}
