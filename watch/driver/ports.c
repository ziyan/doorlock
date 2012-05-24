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
// Button entry functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "driver/ports.h"
#include "driver/buzzer.h"
#include "driver/vti_as.h"
#include "driver/vti_ps.h"
#include "driver/timer.h"
#include "driver/display.h"

// logic
#include "logic/clock.h"
#include "logic/alarm.h"
#include "logic/altitude.h"
#include "logic/stopwatch.h"


// *************************************************************************************************
// Prototypes section
void button_repeat_on(u16 msec);
void button_repeat_off(void);
void button_repeat_function(void);


// *************************************************************************************************
// Defines section

// Macro for button IRQ 
#define IRQ_TRIGGERED(flags, bit)		((flags & bit) == bit)


// *************************************************************************************************
// Global Variable section
volatile s_button_flags button;
volatile struct struct_button sButton;


// *************************************************************************************************
// Extern section
extern void (*fptr_Timer0_A3_function)(void);


// *************************************************************************************************
// @fn          init_buttons
// @brief       Init and enable button interrupts.
// @param       none
// @return      none
// *************************************************************************************************
void init_buttons(void)
{
	// Set button ports to input 
	BUTTONS_DIR &= ~ALL_BUTTONS; 

	// Enable internal pull-downs
	BUTTONS_OUT &= ~ALL_BUTTONS; 
	BUTTONS_REN |= ALL_BUTTONS; 

	// IRQ triggers on rising edge
	BUTTONS_IES &= ~ALL_BUTTONS;   

	// Reset IRQ flags
	BUTTONS_IFG &= ~ALL_BUTTONS;  

	// Enable button interrupts
	BUTTONS_IE |= ALL_BUTTONS;   
}




// *************************************************************************************************
// @fn          PORT2_ISR
// @brief       Interrupt service routine for
//					- buttons M1/M2/S1/S2/BL 
//					- acceleration sensor CMA_INT 
//					- pressure sensor DRDY
// @param       none
// @return      none
// *************************************************************************************************
#pragma vector=PORT2_VECTOR
__interrupt void PORT2_ISR(void)
{
	u8 int_flag, int_enable;
	u8 buzzer = 0;

	// Clear button flags
	button.all_flags = 0;

	// Remember interrupt enable bits
	int_enable = BUTTONS_IE;

	// Store valid button interrupt flag
	int_flag = BUTTONS_IFG & int_enable;

		// Debounce buttons
		if ((int_flag & ALL_BUTTONS) != 0)
		{ 
			// Disable PORT2 IRQ
			__disable_interrupt();
			BUTTONS_IE = 0x00; 
			__enable_interrupt();
	
			// Debounce delay 1
			Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_IN));
	
			// Reset inactivity detection
			sTime.last_activity = sTime.system_time;
			
			// Reset M button high detection
			sTime.previous_m_button_event = sTime.system_time;
		}

		// ---------------------------------------------------
		// M1 button IRQ
		if (IRQ_TRIGGERED(int_flag, BUTTON_M1_PIN))
		{
			// Filter bouncing noise 
			if (BUTTON_M1_IS_PRESSED)
			{
				button.flag.m1 = 1;
				
				sys.flag.mask_m1_button = 0;
		
				// Generate button click
				buzzer = 1;
			}
		}
		// ---------------------------------------------------
		// M2 button IRQ
		else if (IRQ_TRIGGERED(int_flag, BUTTON_M2_PIN))
		{
			// Filter bouncing noise 
			if (BUTTON_M2_IS_PRESSED)
			{
				button.flag.m2 = 1;
				
				sys.flag.mask_m2_button = 0;
	
				// Generate button click
				buzzer = 1;
			}
		}
		// ---------------------------------------------------
		// S1 button IRQ
		else if (IRQ_TRIGGERED(int_flag, BUTTON_S1_PIN))
		{
			// Filter bouncing noise 
			if (BUTTON_S1_IS_PRESSED)
			{
				button.flag.s1 = 1;
		
				// Generate button click
				buzzer = 1;
			}
		}
		// ---------------------------------------------------
		// S2 button IRQ
		else if (IRQ_TRIGGERED(int_flag, BUTTON_S2_PIN))
		{
			// Filter bouncing noise 
			if (BUTTON_S2_IS_PRESSED)
			{
				button.flag.s2 = 1;
	
				// Generate button click
				buzzer = 1;
				
				// Faster reaction for stopwatch stop button press
				if (is_stopwatch()) 
				{
					stop_stopwatch();
					button.flag.s2 = 0;
				}
					
			}
		}
		// ---------------------------------------------------
		// B/L button IRQ
		else if (IRQ_TRIGGERED(int_flag, BUTTON_BL_PIN))
		{
			// Filter bouncing noise 
			if (BUTTON_BL_IS_PRESSED)
			{
				button.flag.bl = 1;
			}
		}	

	// Generate button click when button was activated
	if (buzzer)
	{
		// Any button event stops active alarm
		if (sAlarm.state == ALARM_ON) 
		{
			stop_alarm();
			button.all_flags = 0;
		}
		else if (!sys.flag.s_button_repeat_enabled)
		{
			start_buzzer(1, CONV_MS_TO_TICKS(20), CONV_MS_TO_TICKS(150));
		}
		
		// Debounce delay 2
		Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_OUT));
	}
	
	// ---------------------------------------------------
	// Acceleration sensor IRQ
	if (IRQ_TRIGGERED(int_flag, AS_INT_PIN))
	{
		// Get data from sensor
		request.flag.acceleration_measurement = 1;
  	}
  	
  	// ---------------------------------------------------
	// Pressure sensor IRQ
	if (IRQ_TRIGGERED(int_flag, PS_INT_PIN)) 
	{
		// Get data from sensor
		request.flag.altitude_measurement = 1;
  	}
  	
  	// ---------------------------------------------------
  	// Enable safe long button event detection
  	if(button.flag.m1 || button.flag.m2) 
	{
		// Additional debounce delay to enable safe high detection
		Timer0_A4_Delay(CONV_MS_TO_TICKS(BUTTONS_DEBOUNCE_TIME_M));
	
		// Check if this button event is short enough
		if (BUTTON_M1_IS_PRESSED) button.flag.m1 = 0;
		if (BUTTON_M2_IS_PRESSED) button.flag.m2 = 0;	
	}
	
	// Reenable PORT2 IRQ
	__disable_interrupt();
	BUTTONS_IFG = 0x00; 	
	BUTTONS_IE  = int_enable; 	
	__enable_interrupt();

	// Exit from LPM3/LPM4 on RETI
	__bic_SR_register_on_exit(LPM4_bits); 
}


// *************************************************************************************************
// @fn          button_repeat_on
// @brief       Start button auto repeat timer.
// @param       none
// @return      none
// *************************************************************************************************
void button_repeat_on(u16 msec)
{
	// Set S button repeat flag
	sys.flag.s_button_repeat_enabled = 1;
	
	// Set Timer0_A3 function pointer to button repeat function
	fptr_Timer0_A3_function = button_repeat_function;
	
	// Timer0_A3 IRQ triggers every 200ms
	Timer0_A3_Start(CONV_MS_TO_TICKS(msec));
}


// *************************************************************************************************
// @fn          button_repeat_off
// @brief       Stop button auto repeat timer.
// @param       none
// @return      none
// *************************************************************************************************
void button_repeat_off(void)
{
	// Clear Sx button repeat flag
	sys.flag.s_button_repeat_enabled = 0;
	
	// Timer0_A3 IRQ repeats with 4Hz
	Timer0_A3_Stop();
}


// *************************************************************************************************
// @fn          button_repeat_function
// @brief       Check at regular intervals if button is pushed continuously 
//				and trigger virtual button event.
// @param       none
// @return      none
// *************************************************************************************************
void button_repeat_function(void)
{
	static u8 start_delay = 10;	// Wait for 2 seconds before starting auto up/down
	u8 repeat = 0;
	
	// If buttons S1 or S2 are continuously high, repeatedly set button flag
	if ((BUTTONS_IN & BUTTON_S1_PIN) == BUTTON_S1_PIN)
	{
		if (start_delay == 0)
		{
			// Generate a virtual S1 button event
			button.flag.s1 = 1;
			repeat = 1;
		}
		else
		{
			start_delay--;
		}
	}
	else if ((BUTTONS_IN & BUTTON_S2_PIN) == BUTTON_S2_PIN)
	{
		if (start_delay == 0)
		{
			// Generate a virtual S2 button event
			button.flag.s2 = 1;
			repeat = 1;
		}
		else
		{
			start_delay--;
		}
	}
	else
	{
		// Reset repeat counter
		sButton.repeats = 0;
		start_delay = 10;

		// Enable blinking
		start_blink();
	}
	
	// If virtual button event is generated, stop blinking and reset timeout counter
	if (repeat)
	{
		// Increase repeat counter
		sButton.repeats++;

		// Reset inactivity detection counter
		sTime.last_activity = sTime.system_time;
		
		// Disable blinking
		stop_blink();
	}
}

