// left_side_power -100 ... 100
// right_side_power -100 ... 100
#include <cstring>
#include <stdio.h>

#include "msgParser.h"


bool Parser::parseMessage(const char symbol, EnginesPwr & pwr) { 
    
    printf(
        "parse get new symbol: %c\n", symbol);
    
    switch (state )
    {
        case HEADER:
            printf("case HEADER\n");
            if (symbol == header) {
                state = BODY;
                current_symbol = 0;
                printf("Switch to BODY state:\n");
            }
            break;
        case BODY:
            printf("case BODY. Current symbol: %d\n", current_symbol);
            buffer[current_symbol] = symbol;
            current_symbol++;
            if (current_symbol == msg_size) {
                memcpy(&pwr, buffer, msg_size);
                printf("We collected all symbols. Message: %d %d\n",
                    pwr.left, pwr.right);
                state = HEADER;
                return true;
            }
            break;
    
        default:
            break;
    }
    return false;
}


