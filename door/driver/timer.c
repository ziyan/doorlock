// *************************************************************************************************
// Timer functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section

#include "driver/timer.h"
#include "driver/idle.h"

// *************************************************************************************************
// Define section


// *************************************************************************************************
// Prototype section

void timer_init(void);
u8 timer_running(void);

void timer1_init(void);
void timer1_c0_start(u16 ticks, void (*callback)(void));
void timer1_c0_stop(void);
void timer1_c1_start(u16 ticks, void (*callback)(void));
void timer1_c1_stop(void);
void timer1_c2_start(u16 ticks, void (*callback)(void));
void timer1_c2_stop(void);

void timer2_init(void);
void timer2_delay(u16 msec);

void timer3_init(void);
void timer3_pwm_enable(s8 value);
void timer3_pwm_disable(void);

void timer4_init(void);
void timer4_pwm_enable(u8 value);
void timer4_pwm_disable(void);

// *************************************************************************************************
// Global variables section

void (*timer1_c0_callback)(void) = 0;
void (*timer1_c1_callback)(void) = 0;
void (*timer1_c2_callback)(void) = 0;
u16 timer1_c0_ticks = 0;
u16 timer1_c1_ticks = 0;
u16 timer1_c2_ticks = 0;

void timer_init(void)
{
	timer1_init();
	timer2_init();
	timer3_init();
	timer4_init();
}

u8 timer_running(void)
{
	return ((T1CCTL0 & T1CCTL0_IM) ||
			(T1CCTL1 & T1CCTL1_IM) ||
			(T1CCTL2 & T1CCTL2_IM) ||
			(T2CTL & T2CTL_INT) ||
			(T3CTL & T3CTL_START) ||
			(T4CTL & T4CTL_START));
}

// *************************************************************************************************
// Timer 1

void timer1_init(void)
{
	T1CNTL = 0; // clear T1CNT
	T1CTL = T1CTL_DIV_128 | T1CTL_MODE_FREERUN;
	T1CCTL0 = T1CCTL1 = T1CCTL2 = 0;
	IEN1 |= IEN1_T1IE; // enable global timer 1 interrupt
}

void timer1_c0_start(u16 ticks, void (*callback)(void))
{
	u16 value;
	
	// stop the channel
	timer1_c0_stop();
	
	// set global variables
	timer1_c0_callback = callback;
	timer1_c0_ticks = ticks;
	
	// temporarily pause timer 1
	T1CTL &= ~T1CTL_MODE;
	
	// get current tick value of timer 1
	value = T1CNTL;
	value |= (T1CNTH << 8);
	
	// set compare value on channel 0
	value += ticks;
	T1CC0H = (value >> 8) & 0xFF;
	T1CC0L = value & 0xFF;
	
	// enable interrupt for channel 0
	T1CCTL0 |= T1CCTL0_IM | T1CCTL0_MODE;
	
	// resume timer 1
	T1CTL |= T1CTL_MODE_FREERUN;
}

void timer1_c0_stop(void)
{
	// disable interrupt
	T1CCTL0 &= ~(T1CCTL0_IM | T1CCTL0_MODE);
}

void timer1_c1_start(u16 ticks, void (*callback)(void))
{
	u16 value;
	
	// stop the channel
	timer1_c1_stop();
	
	// set global variables
	timer1_c1_callback = callback;
	timer1_c1_ticks = ticks;
	
	// temporarily pause timer 1
	T1CTL &= ~T1CTL_MODE;
	
	// get current tick value of timer 1
	value = T1CNTL;
	value |= (T1CNTH << 8);
	
	// set compare value on channel 1
	value += ticks;
	T1CC1H = (value >> 8) & 0xFF;
	T1CC1L = value & 0xFF;
	
	// enable interrupt for channel 1
	T1CCTL1 |= T1CCTL1_IM | T1CCTL1_MODE;
	
	// resume timer 1
	T1CTL |= T1CTL_MODE_FREERUN;
}

void timer1_c1_stop(void)
{
	// disable interrupt
	T1CCTL1 &= ~(T1CCTL1_IM | T1CCTL1_MODE);
}

void timer1_c2_start(u16 ticks, void (*callback)(void))
{
	u16 value;
	
	// stop the channel
	timer1_c2_stop();
	
	// set global variables
	timer1_c2_callback = callback;
	timer1_c2_ticks = ticks;
	
	// temporarily pause timer 1
	T1CTL &= ~T1CTL_MODE;
	
	// get current tick value of timer 1
	value = T1CNTL;
	value |= (T1CNTH << 8);
	
	// set compare value on channel 2
	value += ticks;
	T1CC2H = (value >> 8) & 0xFF;
	T1CC2L = value & 0xFF;
	
	// enable interrupt for channel 2
	T1CCTL2 |= T1CCTL2_IM | T1CCTL2_MODE;
	
	// resume timer 1
	T1CTL |= T1CTL_MODE_FREERUN;
}

void timer1_c2_stop(void)
{
	// disable interrupt
	T1CCTL2 &= ~(T1CCTL2_IM | T1CCTL2_MODE);
}


// *************************************************************************************************
// Timer 2
void timer2_init(void)
{

}

void timer2_delay(u16 msec)
{
	// reset timer 2
	IEN1 &= ~IEN1_T2IE;
	T2CTL &= ~T2CTL_INT;
	T2CT = 0;
	
	T2CTL &= ~(T2CTL_TEX | T2CTL_TIG);

	if (msec < 250u)
	{
		// accuracy 0.00136533333 s
		T2CTL |= T2CTL_TIP_64;
		T2PR = 16u;
		T2CT = ((u32)msec * 1000u / 1365u) & 0xFF;
	}
	else if (msec < 500u)
	{
		// accuracy 0.00273066667 s
		T2CTL |= T2CTL_TIP_64;
		T2PR = 32u;
		T2CT = ((u32)msec * 1000u / 2730u) & 0xFF;
	}
	else if (msec < 1000u)
	{
		// accuracy 0.00546133333 s
		T2CTL |= T2CTL_TIP_64;
		T2PR = 64u;
		T2CT = ((u32)msec * 1000u / 5461u) & 0xFF;
	}
	else if (msec < 5000u)
	{
		// accuracy 0.0213333333 s
		T2CTL |= T2CTL_TIP_64;
		T2PR = 250u;
		T2CT = ((u32)msec * 100u / 2133u) & 0xFF;
	}
	else if (msec < 10000u)
	{
		// accurarcy 0.04352 s
		T2CTL |= T2CTL_TIP_128;
		T2PR = 255u;
		T2CT = ((u32)msec * 100u / 4352u) & 0xFF;
	}
	else
	{
		// accuracy 0.349525333 s
		T2CTL |= T2CTL_TIP_1024;
		T2PR = 0; // means 256
		T2CT = ((u32)msec * 10u / 3495u) & 0xFF;
	}

	// reset flag
	sys.flag.delay_over = 0;
	
	// enable interrupt
	T2CTL |= T2CTL_INT;
	IEN1 |= IEN1_T2IE;
	
	for (;;)
	{
		if (sys.flag.delay_over) break;
		
		idle_pm0();
	}
	
	// reset flag
	sys.flag.delay_over = 0;
}

// *************************************************************************************************
// Timer 3

void timer3_init(void)
{
	// Use timer 3 for PWM
	// Enable P1_4 as Timer 3 peripheral
	MCU_IO_OUTPUT(1, 4, 0);
	MCU_IO_PERIPHERAL(1, 4);

	// Prioritize Timer 3 functions on P1
	//P2SEL |= P2SEL_PRI2P1;
	
	// Select alternative 0
	PERCFG &= ~PERCFG_T3CFG;

	// Divide tick clock by 16 (fref = fxosc / 2 = 24MHz, tick spd = fref / 32)
	// Timer modulo mode (counts up to T3CC0 instead of 0xFFFF)
	T3CTL = T3CTL_DIV_64 | T3CTL_MODE_MODULO;
  
	// Channel 1
	// Set when equal to T3CC0, clear when equal to T3CC1, enable compare mode
	T3CCTL1 = T3C1_CLR_CMP_SET_C0 | T3CCTL1_MODE;

	// Servo PWM is 50Hz
	T3CC0 = 234u;

	// lowest value is 12
	// highest value is 24
	// range 12
	// middle value is 18
}

void timer3_pwm_enable(s8 value)
{
	MCU_IO_SET_LOW(1, 4);
	T3CC1 = ((s16)value * 6 / 40 + 18) & 0xFF;
	T3CTL |= T3CTL_CLR;
	T3CTL |= T3CTL_START;
}

void timer3_pwm_disable(void)
{
	T3CTL &= ~T3CTL_START;
	MCU_IO_SET_LOW(1, 4);
}

// *************************************************************************************************
// Timer 4

void timer4_init(void)
{
	// Use timer 4 for PWM
	// Enable P2_0 as Timer 4 peripheral
	MCU_IO_OUTPUT(2, 0, 0);
	MCU_IO_PERIPHERAL(2, 0);
	
	// Select alternative 1
	PERCFG  |= PERCFG_T4CFG;

	// Divide tick clock by 2 (fref = fxosc / 2 = 24MHz, tick spd = fref / 32)
	// 2000 kHz
	// Timer modulo mode (counts up to 0xff)
	T4CTL = T4CTL_DIV_1 | T4CTL_MODE_FREERUN;
  
	// Channel 0
	// Clear on compare, set on 0
	T4CCTL0 = T4CCTL0_CLR_CMP_UP_SET_0 | T4CCTL0_MODE;
}

void timer4_pwm_enable(u8 value)
{
	//MCU_IO_SET_LOW(2, 0);
	T4CC0 = value;
	//T4CTL |= T4CTL_CLR;
	T4CTL |= T4CTL_START;
}

void timer4_pwm_disable(void)
{
	T4CTL &= ~T4CTL_START;
	MCU_IO_SET_LOW(2, 0);
}


// *************************************************************************************************
// Timer 1 Interrupt

#pragma vector = T1_VECTOR
__interrupt void timer1_ISR(void)
{
	// Is it a channel 0 interrupt?
	if (T1CTL & T1CTL_CH0IF)
	{
		// clear the interrupt flag for channel 0
		T1CTL &= ~T1CTL_CH0IF;
		
		//
		// Here we restart the channel 0 before callback as default behavior,
		// in case the callback wish to stop it.
		//
		
		timer1_c0_start(timer1_c0_ticks, timer1_c0_callback);
		timer1_c0_callback();
	}
	
	// Is it a channel 1 interrupt?
	if (T1CTL & T1CTL_CH1IF)
	{
		T1CTL &= ~T1CTL_CH1IF;
		timer1_c1_start(timer1_c1_ticks, timer1_c1_callback);
		timer1_c1_callback();
	}
	
	// Is it a channel 2 interrupt?
	if (T1CTL & T1CTL_CH2IF)
	{
		T1CTL &= ~T1CTL_CH2IF;
		timer1_c2_start(timer1_c2_ticks, timer1_c2_callback);
		timer1_c2_callback();
	}
	
	if (T1CTL & T1CTL_OVFIF)
	{
		T1CTL &= ~T1CTL_OVFIF;
	}
	
	T1IF = 0;
}

#pragma vector = T2_VECTOR
__interrupt void timer2_ISR(void)
{
	if (T2CTL & T2CTL_TEX)
	{
		// reset timer 2
		IEN1 &= ~IEN1_T2IE;
		T2CTL &= ~T2CTL_INT;
		T2CT = 0;
			
		// clear
		T2CTL &= ~T2CTL_TEX;
			
		sys.flag.delay_over = 1;
	}
}
