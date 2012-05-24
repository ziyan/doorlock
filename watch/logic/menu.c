// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
//	 
//	 
//	  Redistribution and use in source and binary forms, with or without 
//	  modification, are permitted provided that the following conditions 
//	  are met:
//	
//	    Redistributions of source code must retain the above copyright 
//	    notice, this list of conditions and the following disclaimer.
//	 
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the 
//	    documentation and/or other materials provided with the   
//	    distribution.
//	 
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//	
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Menu management functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "driver/display.h"

// logic
#include "logic/menu.h"
#include "logic/user.h"
#include "logic/clock.h"
#include "logic/date.h"
#include "logic/alarm.h"
#include "logic/stopwatch.h"
#include "logic/temperature.h"
#include "logic/altitude.h"
#include "logic/battery.h"
#include "logic/acceleration.h"
#include "logic/doorlock.h"

// *************************************************************************************************
// Defines section
#define FUNCTION(function)  function


// *************************************************************************************************
// Global Variable section
const struct menu * ptrMenu_L1 = NULL;
const struct menu * ptrMenu_L2 = NULL;

// Function pointers for LINE1 and LINE2 display function 
void (*fptr_lcd_function_line1)(u8 line, u8 update);
void (*fptr_lcd_function_line2)(u8 line, u8 update);


// *************************************************************************************************
// Global Variable section

void display_nothing(u8 line, u8 update) {}

u8 update_time(void)
{
	return (display.flag.update_time);
}
u8 update_stopwatch(void)
{
	return (display.flag.update_stopwatch);
}
u8 update_date(void)
{
	return (display.flag.update_date);
}
u8 update_alarm(void)
{
	return (display.flag.update_alarm);
}
u8 update_temperature(void)
{
	return (display.flag.update_temperature);
}
u8 update_battery_voltage(void)
{
	return (display.flag.update_battery_voltage);
}
u8 update_acceleration(void)
{
	return (display.flag.update_acceleration);
}
u8 update_doorlock(void)
{
	return (display.flag.update_doorlock);
}

// *************************************************************************************************
// User navigation ( [____] = default menu item after reset )
//
//	LINE1: 	[Time] -> Alarm -> Temperature -> Altitude -> Heart rate -> Speed -> Acceleration
//
//	LINE2: 	[Date] -> Stopwatch -> Battery -> Doorlock
// *************************************************************************************************

// Line1 - Time
const struct menu menu_L1_Time =
{
	FUNCTION(sx_time),			// direct function
	FUNCTION(mx_time),			// sub menu function
	FUNCTION(display_time),		// display function
	FUNCTION(update_time),		// new display data
	&menu_L1_Alarm,
};
// Line1 - Alarm
const struct menu menu_L1_Alarm =
{
	FUNCTION(sx_alarm),			// direct function
	FUNCTION(mx_alarm),			// sub menu function
	FUNCTION(display_alarm),	// display function
	FUNCTION(update_alarm),		// new display data
	&menu_L1_Temperature,
};
// Line1 - Temperature
const struct menu menu_L1_Temperature =
{
	FUNCTION(dummy),					// direct function
	FUNCTION(mx_temperature),			// sub menu function
	FUNCTION(display_temperature),		// display function
	FUNCTION(update_temperature),		// new display data
	&menu_L1_Altitude,
};
// Line1 - Altitude
const struct menu menu_L1_Altitude =
{
	FUNCTION(sx_altitude),				// direct function
	FUNCTION(mx_altitude),				// sub menu function
	FUNCTION(display_altitude),			// display function
	FUNCTION(update_time),				// new display data
	&menu_L1_Acceleration,
};
// Line1 - Acceleration
const struct menu menu_L1_Acceleration =
{
	FUNCTION(sx_acceleration),			// direct function
	FUNCTION(dummy),					// sub menu function
	FUNCTION(display_acceleration),		// display function
	FUNCTION(update_acceleration),		// new display data
	&menu_L1_Time,
};

// Line2 - Date
const struct menu menu_L2_Date =
{
	FUNCTION(sx_date),			// direct function
	FUNCTION(mx_date),			// sub menu function
	FUNCTION(display_date),		// display function
	FUNCTION(update_date),		// new display data
	&menu_L2_Stopwatch,
};
// Line2 - Stopwatch
const struct menu menu_L2_Stopwatch =
{
	FUNCTION(sx_stopwatch),		// direct function
	FUNCTION(mx_stopwatch),		// sub menu function
	FUNCTION(display_stopwatch),// display function
	FUNCTION(update_stopwatch),	// new display data
	&menu_L2_Battery,
};
// Line2 - Battery 
const struct menu menu_L2_Battery =
{
	FUNCTION(dummy),					// direct function
	FUNCTION(dummy),					// sub menu function
	FUNCTION(display_battery_V),		// display function
	FUNCTION(update_battery_voltage),	// new display data
	&menu_L2_Doorlock,
};
// Line2 - Doorlock 
const struct menu menu_L2_Doorlock =
{
	FUNCTION(sx_doorlock),					// direct function
	FUNCTION(mx_doorlock),					// sub menu function
	FUNCTION(display_doorlock),				// display function
	FUNCTION(update_doorlock),				// new display data
	&menu_L2_Date,
};
