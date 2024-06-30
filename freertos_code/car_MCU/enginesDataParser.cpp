// left_side_power -100 ... 100
// right_side_power -100 ... 100
#include <cstring>
#include <stdio.h>

#include "enginesDataParser.h"



bool EnginesDataParser::parse(const char symbol, EnginesPwr & e) { 
    
    printf(
        "parse get new symbol: %c\n", symbol);
    
    switch (state )
    {
        case HEADER:
            printf("case HEADER\n");
            header_buff[0] = header_buff[1];
            header_buff[1] = symbol;
            if (0 == compareCharArrays(header_buff, UART_ENGINES_DATA_HEADER)) {
                state = BODY;
                header_buff[0] = '0'; header_buff[1] = '0';
                counter = 0;
                printf("Switch to BODY state:\n");
            }
            break;
        case BODY:
            printf("case BODY. Current symbol: %d\n", counter);
            body_buff[counter] = symbol;
            counter++;
            if (counter == body_size) {
                memcpy(&e, body_buff, body_size);
                printf("We collected all symbols. Message: %d %d\n",
                    e.left, e.right);
                state = HEADER;
                return true;
            }
            break;
    
        default:
            break;
    }
    return false;
}

int EnginesDataParser::compareCharArrays(const char arr1[2], const char arr2[2]) {
    for (int i = 0; i < 2; ++i) {
        if (arr1[i] != arr2[i]) {
            return (arr1[i] < arr2[i]) ? -1 : 1;
        }
    }
    return 0; // Arrays are equal
}
