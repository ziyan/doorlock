// *************************************************************************************************
// This file defines interface for clock related functions for the CC1110/CC2510 family of
// RF system-on-chips from Texas Instruments.
// Copyright 2007 Texas Instruments, Inc.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "driver/clock.h"


// *************************************************************************************************
// Define section


// *************************************************************************************************
// Prototype section
void clock_set_main_src(u8 source);

// *************************************************************************************************
// @fn		clockSetMainSrc
// @brief	Function for setting the main system clock source.
//			The function turns off the clock source that is not being used.
//			TICKSPD is set to the same frequency as the source.
// @param	u8 source (one of CLOCK_SRC_HFRC or CLOCK_SRC_XOSC)
// @return	void
// *************************************************************************************************
void clock_set_main_src(u8 source)
{
	// Source can have the following values:
	// CLOCK_SRC_XOSC   0x00  /*  High Speed Crystal Oscillator (XOSC) */
	// CLOCK_SRC_HFRC   0x01  /*  High Speed RC Oscillator (HS RCOSC) */
	if (source == CLOCK_SRC_HFRC)
	{
		SLEEP &= ~SLEEP_OSC_PD;				// power up both oscillators
		while (!(SLEEP & SLEEP_HFRC_S));	// wait till HFRC is stable
		CLKCON |= CLKCON_OSC;				// change system clock source to HS RCOSC
		while (!(CLKCON & CLKCON_OSC));		// wait until CLKCON.OSC = 1 
											// (system clock running on HS RCOSC)
		CLKCON = (CLKCON_OSC32 | CLKCON_OSC | TICKSPD_DIV_32 | CLKSPD_DIV_2);
		while (CLKCON != (CLKCON_OSC32 | CLKCON_OSC | TICKSPD_DIV_32 | CLKSPD_DIV_2));
		SLEEP |= SLEEP_OSC_PD;				// power down the unused oscillator
	}
	else if (source == CLOCK_SRC_XOSC)
	{
		SLEEP &= ~SLEEP_OSC_PD;				// power up both oscillators
		while (!(SLEEP & SLEEP_XOSC_S));	// wait till XOSC is stable
		CLKCON &= ~CLKCON_OSC;
		while (CLKCON & CLKCON_OSC);		// wait until CLKCON.OSC = 0 
											// (system clock running on HS XOSC)
		
		CLKCON = CLKCON_OSC32 | TICKSPD_DIV_32 | CLKSPD_DIV_1;
		while (CLKCON != (CLKCON_OSC32 | TICKSPD_DIV_32 | CLKSPD_DIV_1));
		SLEEP |= SLEEP_OSC_PD;				// power down the unused oscillator
	}
}
