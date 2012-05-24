// *************************************************************************************************
// Include section
#include "driver/timer.h"
#include "driver/ir.h"
#include "driver/buzzer.h"
#include "logic/door.h"

// *************************************************************************************************
// Define section
#define DOOR_TIMER_INTERVAL			(CONV_MS_TO_TICKS(500))
#define DOOR_IR_THRESHOLD			(50)
#define DOOR_IR_AVG_WINDOW_SIZE		(4)
#define DOOR_OPEN_MAX_LENGTH		(240u)
#define DOOR_CLOSE_MAX_LENGTH		(40u)

// *************************************************************************************************
// Prototype section
void door_init(void);
void door_start(void);
void door_timer(void);
void door_update(void);
void door_stop(void);

// *************************************************************************************************
// Global variables section
s16 door_ir_avg = 0;
u16 door_open_length = 0;
u8 door_close_length = 0;

void door_init(void)
{
	state.flag.door = DOOR_STATE_CLOSED;
}

void door_start(void)
{
	door_ir_avg = 25;
	door_open_length = 0;
	door_close_length = 0;
	timer1_c2_start(DOOR_TIMER_INTERVAL, door_timer);
}

void door_timer(void)
{
	request.flag.door_update = 1;
}

void door_update(void)
{
	// check ir range
	s8 value = ir_sample(IR_DOOR);
	door_ir_avg *= DOOR_IR_AVG_WINDOW_SIZE - 1;
	door_ir_avg += value;
	door_ir_avg /= DOOR_IR_AVG_WINDOW_SIZE;

	// threshold value of door state
	if (door_ir_avg < DOOR_IR_THRESHOLD)
	{
		DOOR_STATE(DOOR_STATE_OPEN);
	}
	else
	{
		DOOR_STATE(DOOR_STATE_CLOSED);
	}
	
	// see how long door's been open
	if (state.flag.door == DOOR_STATE_OPEN)
	{
		if (door_open_length < DOOR_OPEN_MAX_LENGTH)
		{
			++door_open_length;
		}
		else
		{
			// if door has been hold too long
			// start buzzer to annoy people
			buzzer_on();
		}
		door_close_length = 0;
	}
	else
	{
		if (door_open_length >= DOOR_OPEN_MAX_LENGTH)
		{
			// if door has previously been hold too long
			// stop buzzer
			buzzer_off();
		}
		door_open_length = 0;
		
		if (door_close_length < DOOR_CLOSE_MAX_LENGTH)
		{
			++door_close_length;
			if (door_close_length >= DOOR_CLOSE_MAX_LENGTH)
			{
				// if door has closed for a while
				// lock it
				request.flag.lock_lock = 1;
			}
		}
	}
}

void door_stop(void)
{
	buzzer_off();
	timer1_c2_stop();
}

