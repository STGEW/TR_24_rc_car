#pragma once

#include "../const.h"

/*
    USED DRIVER: tb6612fng

    AI1 (BI1)       AI2 (BI2)           Direction
    LOW             LOW                 OFF
    HIGH            LOW                 FORWARD
    LOW             HIGH                REVERSE
    HIGH            HIGH                BRAKE

    STBY PIN - always HIGH
*/

#define FORWARD         1
#define OFF             2
#define REVERSE         3
#define BRAKE           4

#define DRIVER_COEF     65535 / 100

void setupMotorDriver( void );
void runMotorDriver( DriverControlData & data );
