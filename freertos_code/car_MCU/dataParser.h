#pragma once

#include <cstdint> // ??? Do I need it?
#include <cstddef> 
#include "../const.h"

#define body_size       sizeof(PathPlanningData)
#define header_size     sizeof(UART_PP_DATA_HEADER)

enum ParserState {
    HEADER,
    BODY
};

class DataParser {

public:
    bool parse(const char symbol, PathPlanningData & e);

private:

    int compareCharArrays(const char arr1[2], const char arr2[2]);

    ParserState state = HEADER;
    size_t counter = 0;
    char header_buff[header_size];
    char body_buff[body_size];
};
