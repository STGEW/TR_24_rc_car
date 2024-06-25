#pragma once

#include <cstdint>

#include "../const.h"

// left_side_power -100 ... 100
// right_side_power -100 ... 100

const char header = 'A';
const size_t msg_size = sizeof(EnginesPwr);

enum ParserState {
    HEADER,
    BODY
};


class Parser {
public:
    bool parseMessage(const char symbol, EnginesPwr & pwr);
private:
    ParserState state = HEADER;
    size_t current_symbol = 0;
    char buffer[msg_size];
};
