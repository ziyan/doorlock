// *************************************************************************************************
// Include section
#include "driver/servo.h"
#include "driver/timer.h"
#include "logic/lock.h"

// *************************************************************************************************
// Define section
#define LOCK_TIMER_INTERVAL	(CONV_MS_TO_TICKS(5))
#define LOCK_UNLOCK_TIME	(400u)						// set max to 2 seconds
#define LOCK_LOCK_TIME		(500u)						// set max to 2.5 seconds (to be sure)

// *************************************************************************************************
// Prototype section
void lock_init(void);
void lock_unlock(void);
void lock_timer(void);
void lock_lock(void);
void lock_update(void);
void lock_stop(void);

// *************************************************************************************************
// Global variables section
volatile u16 lock_time = 0;

void lock_init(void)
{
	//lock_unlock();
	state.flag.lock = LOCK_STATE_LOCKED;
}

void lock_unlock(void)
{
	LOCK_STATE(LOCK_STATE_UNLOCKING);
	lock_time = LOCK_UNLOCK_TIME;
	servo_enable(SERVO_POSITION_UNLOCKED);
	timer1_c2_start(LOCK_TIMER_INTERVAL, lock_timer);
}

void lock_timer(void)
{
	request.flag.lock_update = 1;
}

void lock_lock(void)
{
	LOCK_STATE(LOCK_STATE_LOCKING);
	lock_time = LOCK_LOCK_TIME;
	servo_enable(SERVO_POSITION_LOCKED);
	timer1_c2_start(LOCK_TIMER_INTERVAL, lock_timer);
}

void lock_update(void)
{
	if (!lock_time || servo_is_stalled())
	{
		timer1_c2_stop();
		servo_disable();
		if (state.flag.lock == LOCK_STATE_UNLOCKING)
		{
			state.flag.lock = LOCK_STATE_UNLOCKED;
			request.flag.lock_changed = 1;
		}
		else if (state.flag.lock == LOCK_STATE_LOCKING)
		{
			if (P0_4)
			{
				// the reed switch shows that we are in locked position
				state.flag.lock = LOCK_STATE_LOCKED;
				request.flag.lock_changed = 1;
			}
			else
			{
				// we are timing out but still cannot reach the lock position
				// meaning deadbolt has stucked probably, so unlock to fix the problem
				lock_unlock();
			}
		}
	}
	else
	{
		--lock_time;
	}
}

void lock_stop(void)
{
	timer1_c2_stop();
	servo_disable();
}
