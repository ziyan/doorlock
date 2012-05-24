// *************************************************************************************************
// Buzzer functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "driver/buzzer.h"
#include "driver/timer.h"

// *************************************************************************************************
// Define section
#define BUZZER_FREQ	(127u)	// 50% duty cycle

// *************************************************************************************************
// Prototype section
void buzzer_start(u8 repeat, u16 on_ticks, u16 off_ticks, u8 led);
void buzzer_stop(void);
void buzzer_timer(void);
void buzzer_on(void);
void buzzer_off(void);
void buzzer_pause(void);
void buzzer_resume(void);

// *************************************************************************************************
// Global variable section
volatile u16 buzzer_repeat = 0; // odd is on, even is off, this = repeat * 2 + 1
volatile u16 buzzer_on_ticks = 0;
volatile u16 buzzer_off_ticks = 0;
volatile u8 buzzer_options = 0;

void buzzer_start(u8 repeat, u16 on_ticks, u16 off_ticks, u8 options)
{
	buzzer_stop();

	buzzer_repeat = (u16)repeat;
	buzzer_repeat <<= 1;
	buzzer_repeat |= 1u; // make sure it is odd
	buzzer_on_ticks = on_ticks;
	buzzer_off_ticks = off_ticks;
	buzzer_options = options;

	buzzer_on();
	if (buzzer_options & BUZZER_OPTION_TOGGLE_LED)
	{
		MCU_IO_SET_HIGH(0, 5);
	}

	timer1_c0_start(buzzer_on_ticks, buzzer_timer);
}

void buzzer_stop(void)
{
	buzzer_pause();

	// clear settings
	if (buzzer_options & BUZZER_OPTION_TOGGLE_LED)
	{
		MCU_IO_SET_LOW(0, 5);
	}
	buzzer_repeat = 0;
	buzzer_on_ticks = 0;
	buzzer_off_ticks = 0;
	buzzer_options = 0;
}

void buzzer_timer(void)
{
	buzzer_resume();
}

void buzzer_on(void)
{
	timer4_pwm_enable(BUZZER_FREQ);
}

void buzzer_off(void)
{
	timer4_pwm_disable();
}

void buzzer_pause(void)
{
	buzzer_off();
	timer1_c0_stop();
}

void buzzer_resume(void)
{
	timer1_c0_stop();

	if (!buzzer_repeat)
	{
		buzzer_stop();
	}
	else
	{
		--buzzer_repeat;
		if (buzzer_repeat & 1u)
		{
			// odd means on
			buzzer_on();
			if (buzzer_options & BUZZER_OPTION_TOGGLE_LED)
			{
				MCU_IO_SET_HIGH(0, 5);
			}
			timer1_c0_start(buzzer_on_ticks, buzzer_timer);
		}
		else
		{
			// even means off
			buzzer_off();
			if (buzzer_options & BUZZER_OPTION_TOGGLE_LED)
			{
				MCU_IO_SET_LOW(0, 5);
			}
			timer1_c0_start(buzzer_off_ticks, buzzer_timer);
		}
	}
}