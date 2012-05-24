// *************************************************************************************************
// Initialization and control of sys.
// *************************************************************************************************

// *************************************************************************************************
// Include section

#include "project.h"

#include "driver/clock.h"
#include "driver/ports.h"
#include "driver/radio.h"
#include "driver/adc.h"
#include "driver/timer.h"
#include "driver/idle.h"
#include "driver/buzzer.h"
#include "driver/ir.h"
#include "driver/servo.h"

#include "logic/lock.h"
#include "logic/door.h"
#include "logic/db.h"
#include "logic/packet.h"
#include "logic/auth.h"
#include "logic/mode.h"

#include <string.h>

// *************************************************************************************************
// Prototype section

void init_application(void);
void process_sys(void);
void process_radio(void);
void process_request(void);

// *************************************************************************************************
// Global variable section

volatile request_flags_t request;
volatile sys_flags_t sys;
volatile radio_flags_t radio;
volatile state_flags_t state;

// *************************************************************************************************
// @fn          main
// @brief       Main routine
// @param       none
// @return      none
// *************************************************************************************************
int main(void)
{ 	
	init_application();

	for(;;)
	{
		// low power mode
		if (!sys.all_flags && !radio.all_flags && !request.all_flags)
		{
			if (timer_running() || !RADIO_IDLE)
			{
				idle_pm0();
			}
			else
			{
				idle_pm2();

				MODE_STATE(MODE_STATE_NORMAL);
				radio_init();
				radio_receive();
			}
		}

		// process system events
		if (sys.all_flags) process_sys();
		
		// process radio events
		if (radio.all_flags) process_radio();

		// process request
		if (request.all_flags) process_request();
	}
}

// *************************************************************************************************
// @fn          init_application
// @brief       Initialize the microcontroller.
// @param       none
// @return      none
// *************************************************************************************************
void init_application(void)
{
	request.all_flags = 0;
	sys.all_flags = 0;
	radio.all_flags = 0;
	state.all_flags = 0;

	// Setup clock as high speed crystal oscillator
	clock_set_main_src(CLOCK_SRC_XOSC);
	
	// sleep timer setup
	idle_init();

	// drivers init
	ports_init();
	timer_init();
	adc_init();
	ir_init();
	servo_init();
	radio_init();

	// Enable global interrupts
	EA = 1;

	// logic init
	db_init();
	door_init();
	auth_init();
	lock_init();
	mode_init();

	radio_receive();
}


// *************************************************************************************************
// @fn          process_sys
// @brief       Events include button, reed switch and sleep timer events
// @param       none
// @return      none
// *************************************************************************************************
void process_sys(void)
{
	if (sys.flag.sleep_wakeup)
	{
		sys.flag.sleep_wakeup = 0;
	}

	if (sys.flag.lock)
	{
		sys.flag.lock = 0;

		// door manually locked
		if (state.flag.lock == LOCK_STATE_UNLOCKED)
		{
			state.flag.lock = LOCK_STATE_LOCKED;
			request.flag.lock_changed = 1;
		}
	}

	if (sys.flag.unlock)
	{
		sys.flag.unlock = 0;

		// door manually unlocked
		if (state.flag.lock == LOCK_STATE_LOCKED)
		{
			state.flag.lock = LOCK_STATE_UNLOCKED;
			request.flag.lock_changed = 1;
		}
	}

	if (sys.flag.button)
	{
		sys.flag.button = 0;

		// button short press
		if (state.flag.mode == MODE_STATE_NORMAL)
		{
			// lock / unlock button
			if (state.flag.lock == LOCK_STATE_LOCKED)
			{
				request.flag.lock_unlock = 1;
			}
			else if (state.flag.lock == LOCK_STATE_UNLOCKED && state.flag.door == DOOR_STATE_CLOSED)
			{
				// only lock the door when the door is closed
				request.flag.lock_lock = 1;
			}
		}
		else
		{
			// exit from other mode
			MODE_STATE(MODE_STATE_NORMAL);
		}
	}

	if (sys.flag.button_long)
	{
		sys.flag.button_long = 0;
		
		// button long press
		if (state.flag.mode == MODE_STATE_NORMAL)
		{
			// go into passwd state
			MODE_STATE(MODE_STATE_PASSWD);
		}
		else if (state.flag.mode == MODE_STATE_PASSWD)
		{
			// go into pair mode
			MODE_STATE(MODE_STATE_PAIR);
		}
		else
		{
			// clear db
			MODE_STATE(MODE_STATE_NORMAL);
			db_clear();
		}
	}
}

// *************************************************************************************************
// @fn          process_radio
// @brief       Process requested actions outside ISR context.
// @param       none
// @return      none
// *************************************************************************************************
void process_radio(void)
{
	packet_t packet = {0};
	
	if (radio.flag.error)
	{
		radio.flag.error = 0;

		// got a radio error, continue rx
		radio_receive();
	}
	
	if (radio.flag.rx)
	{
		radio.flag.rx = 0;
		
		// retrieve the packet
		radio_retrieve(&packet);

		// crc check
		if (packet.status.crc_ok && packet.version == PACKET_VERSION)
		{
			if (auth_process_packet(&packet))
			{
				radio_receive();
			}
			else
			{
				radio_transmit(&packet);
			}
		}
		else
		{
			// crc is not correct
			// ignore the packet and continue receiving
			radio_receive();
		}
	}
	
	if (radio.flag.tx)
	{
		// packet transmit successfully
		radio.flag.tx = 0;
		radio_receive();
	}

	if (radio.flag.timeout)
	{
		// rx timeout
		radio.flag.timeout = 0;
		while (!RADIO_IDLE);
		if (timer_running())
			radio_receive();
	}
}

// *************************************************************************************************
// @fn          process_request
// @brief       Process requests generated by logic
// @param       none
// @return      none
// *************************************************************************************************
void process_request(void)
{
	// servo current sampling
	if (request.flag.lock_update)
	{
		request.flag.lock_update = 0;
		lock_update();
	}
	
	// door ir sampling
	if (request.flag.door_update)
	{
		request.flag.door_update = 0;
		door_update();
	}
	
	// request for unlocking
	if (request.flag.lock_unlock)
	{
		request.flag.lock_unlock = 0;

		// only unlock when it is not in unlocked state
		if (state.flag.lock != LOCK_STATE_UNLOCKED)
		{
			lock_unlock();
		}
	}
	
	// request for locking
	if (request.flag.lock_lock)
	{
		request.flag.lock_lock = 0;

		// only lock when it is in unlocked state
		if (state.flag.lock != LOCK_STATE_LOCKED && state.flag.door == DOOR_STATE_CLOSED)
		{
			lock_lock();
		}
	}
	
	// lock state transition
	if (request.flag.lock_changed)
	{
		request.flag.lock_changed = 0;
		
		// the state of the lock has changed
		if (state.flag.lock == LOCK_STATE_UNLOCKED)
		{
			// door is now unlocked, either manually or by servo
			// this ensures that the servo and the timer is stopped
			lock_stop();
			// start door state ir sensing
			door_start();
		}
		else if (state.flag.lock == LOCK_STATE_LOCKED)
		{
			// door is now locked, either manually or by servo
			// this ensures that the servo and the timer is stopped
			lock_stop();
			// this ensures if we transit from unlock to lock directly
			// if someone manually lock the door, we will stop looking
			// for door close
			door_stop();
		}
	}
	
	// door state transition
	if (request.flag.door_changed)
	{
		request.flag.door_changed = 0;
		
		if (state.flag.door == DOOR_STATE_OPEN)
		{
			// door is now open
		}
		else
		{
			// door is now closed
		}
	}
	
	// mode state transition
	if (request.flag.mode_changed)
	{
		request.flag.mode_changed = 0;
		
		mode_update();
	}
}

