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

#ifndef _DRIVER_BUTTONS_H_
#define _DRIVER_BUTTONS_H_


// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Defines section

// Port, pins and interrupt resources for buttons
#define BUTTONS_IN              (P2IN)
#define BUTTONS_OUT				(P2OUT)
#define BUTTONS_DIR             (P2DIR)
#define BUTTONS_REN				(P2REN)
#define BUTTONS_IE              (P2IE)
#define BUTTONS_IES             (P2IES)
#define BUTTONS_IFG             (P2IFG)
#define BUTTONS_IRQ_VECT2       (PORT2_VECTOR)

// Button ports
#define BUTTON_M1_PIN          	(BIT2)
#define BUTTON_M2_PIN          	(BIT1)
#define BUTTON_S1_PIN          	(BIT4)
#define BUTTON_S2_PIN          	(BIT0)
#define BUTTON_BL_PIN          	(BIT3)
#define ALL_BUTTONS			 	(BUTTON_M1_PIN + BUTTON_M2_PIN + BUTTON_S1_PIN + BUTTON_S2_PIN + BUTTON_BL_PIN)

// Macros for button press detection
#define BUTTON_M1_IS_PRESSED	((BUTTONS_IN & BUTTON_M1_PIN) == BUTTON_M1_PIN)
#define BUTTON_M2_IS_PRESSED	((BUTTONS_IN & BUTTON_M2_PIN) == BUTTON_M2_PIN)
#define BUTTON_S1_IS_PRESSED	((BUTTONS_IN & BUTTON_S1_PIN) == BUTTON_S1_PIN)
#define BUTTON_S2_IS_PRESSED	((BUTTONS_IN & BUTTON_S2_PIN) == BUTTON_S2_PIN)
#define BUTTON_BL_IS_PRESSED	((BUTTONS_IN & BUTTON_BL_PIN) == BUTTON_BL_PIN)
#define NO_BUTTON_IS_PRESSED	((BUTTONS_IN & ALL_BUTTONS) == 0)

// Macros for button release detection
#define BUTTON_M1_IS_RELEASED	((BUTTONS_IN & BUTTON_M1_PIN) == 0)
#define BUTTON_M2_IS_RELEASED	((BUTTONS_IN & BUTTON_M2_PIN) == 0)
#define BUTTON_S1_IS_RELEASED	((BUTTONS_IN & BUTTON_S1_PIN) == 0)
#define BUTTON_S2_IS_RELEASED	((BUTTONS_IN & BUTTON_S2_PIN) == 0)
#define BUTTON_BL_IS_RELEASED	((BUTTONS_IN & BUTTON_BL_PIN) == 0)

// Button debounce time (msec)
#define BUTTONS_DEBOUNCE_TIME_IN	(5u)
#define BUTTONS_DEBOUNCE_TIME_OUT	(250u)
#define BUTTONS_DEBOUNCE_TIME_M		(50u)

// Detect if M1/M2 button is held low continuously  
#define M_BUTTON_LONG_TIME		(2u)

// Recovery time between adjacent button events (seconds)
#define BUTTON_MASK_TIME		(1u)

// Leave set_value() function after some seconds of user inactivity
#define INACTIVITY_TIME			(30u)


// Set of button flags
typedef union
{
  struct
  {
  	// Manual button events
    u16 m1     		: 1;    // Short M1 button press
    u16 m2     		: 1;    // Short M2 button press
    u16 s1      	: 1;    // Short S1 button press
    u16 s2      	: 1;    // Short S2 button press
    u16 bl      	: 1;    // Short BL button press
    u16 m1_long     : 1;    // Long M1 button press
    u16 m2_long    	: 1;    // Long M2 button press
  } flag;
  u16 all_flags;            // Shortcut to all display flags (for reset)
} s_button_flags;
extern volatile s_button_flags button;

struct struct_button
{
	u8  m1_timeout;			// 
	u8  m2_timeout;			// 
	s16 repeats;			//
};
extern volatile struct struct_button sButton;

// *************************************************************************************************
// Extern section
extern void button_repeat_on(u16 msec);
extern void button_repeat_off(void);
extern void button_repeat_function(void);
extern void init_buttons(void);


#endif /*_DRIVER_BUTTONS_H_*/
