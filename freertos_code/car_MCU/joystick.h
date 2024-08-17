#include "../const.h"
#include <RF24.h>

// Conversion from joystick to engine state
/*
What we get from joystick?
0..4095 as a full range.

Regarding joystick position:
0..2047 - left
2048..4095 - right

How we interpret these values?
2048 - DEAD_ZONE ... 2048 + DEAD_ZONE - safe zone / dead zone / neutral

0 ... 2048 - DEAD_ZONE - 'backward'/'left'
2048 + DEAD_ZONE ... 4095 - 'forward'/'right'
*/
#define DEAD_ZONE               50
#define DEAD_ZONE_START         2048 - DEAD_ZONE
#define DEAD_ZONE_END           2048 + DEAD_ZONE
#define TURN_COEF               0.60    // this coefficient is chosen after several experiments
#define ADC_TO_PERCENTAGE       100.0 / 2048.0


class Joystick {
public:
	Joystick();
	bool receive(EnginesPwr &engines_pwr, bool &ext_ctrl);

private:
	RF24 radio = RF24(CE_PIN, CSN_PIN);
	bool role = false; // true = TX role, false = RX role
	JoystickRawData joystick;

	void convToEnginesPwrs(JoystickRawData & j, EnginesPwr & pwr);
};
