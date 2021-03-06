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

#ifndef _DRIVER_TIMER_H_
#define _DRIVER_TIMER_H_

// *************************************************************************************************
// Include section
#include "project.h"


// *************************************************************************************************
// Prototypes section
extern void Timer0_Init(void);
extern void Timer0_Start(void);
extern void Timer0_Stop(void);
extern void Timer0_A1_Start(u16 ticks);
extern void Timer0_A1_Stop(void);
extern void Timer0_A3_Start(u16 ticks);
extern void Timer0_A3_Stop(void);
extern void Timer0_A4_Delay(u16 ticks);
extern void (*fptr_Timer0_A1_function)(void);
extern void (*fptr_Timer0_A3_function)(void);



// *************************************************************************************************
// Defines section
struct timer
{
	// Timer0_A1 periodic delay
	u16		timer0_A1_ticks;
	// Timer0_A3 periodic delay 
	u16		timer0_A3_ticks;
};
extern struct timer sTimer;

// Trigger reset when all buttons are pressed
#define BUTTON_RESET_SEC		(3u)

// *************************************************************************************************
// Global Variable section


// *************************************************************************************************
// Extern section



#endif /*_DRIVER_TIMER_H_*/

