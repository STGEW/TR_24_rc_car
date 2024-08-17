#pragma once

// TBD - add a checksum in the end

#include <cstdint> // ??? Do I need it?
#include <cstddef> 
#include <assert.h>
#include "../const.h"

// commands
const char CMDS_HEADER[] = {'T', 'R'};
const char PP_HEADER[] = {'L', 'F'};

static_assert(sizeof(CMDS_HEADER) == sizeof(PP_HEADER), "Size of CMDS_HEADER and PP_HEADER must be equal");


#define SIZE_VAR_SIZE       1
#define MAX_BODY_SIZE       256
#define HEADER_SIZE         sizeof(CMDS_HEADER)


enum ParserState {
    HEADER,
    SIZE,
    BODY
};

class CommandsParser {

public:

    CommandsParser(
        void (*_new_path_cb_ptr)(Point2D * point_2D),
        void (*_stop_cb_ptr)(void));

    void reset();
    void parse(const char symbol);

private:

    int compareCharArrays(
        const char arr1[sizeof(CMDS_HEADER)],
        const char arr2[sizeof(CMDS_HEADER)]);

    Point2D point_2D;
    ParserState state = HEADER;
    size_t counter = 0;
    size_t exp_size = 0;
    char header_buff[HEADER_SIZE];
    char body_buff[MAX_BODY_SIZE];
    void (*new_path_cb_ptr)(Point2D * point_2D);
    void (*stop_cb_ptr)(void);
    bool is_cmd = false;
};
