// *************************************************************************************************
// Include section
#include "driver/buzzer.h"
#include "driver/timer.h"
#include "logic/mode.h"

// *************************************************************************************************
// Define section

// *************************************************************************************************
// Prototype section
void mode_init(void);
void mode_update(void);

// *************************************************************************************************
// Global variables section

void mode_init(void)
{
	state.flag.mode = MODE_STATE_NORMAL;
}

void mode_update(void)
{
	if (state.flag.mode == MODE_STATE_NORMAL)
	{
		buzzer_stop();
	}
	else if (state.flag.mode == MODE_STATE_PASSWD)
	{
		// in passwd mode, we need an alert sound
		buzzer_start(0xFFu, CONV_MS_TO_TICKS(100), CONV_MS_TO_TICKS(1900), BUZZER_OPTION_TOGGLE_LED);
	}
	else
	{
		// in pair mode, we need somewhat more annoying sound
		buzzer_start(0xFFu, CONV_MS_TO_TICKS(250), CONV_MS_TO_TICKS(750), BUZZER_OPTION_TOGGLE_LED);
	}
}

