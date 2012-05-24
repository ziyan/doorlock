// *************************************************************************************************
// Ports configuration
//
// The CC1111 USB Dongle breaks out the following ports:
// - DEBUG
//   -  1: GND
//   -  2: VDD
//   -  3: P2_2 / Debug Clock
//   -  4: P2_1 / Debug Data
//   -  5: P1_4
//   -  6: P1_5
//   -  7: RESET_N
//   -  8: P1_6
//   -  9: NC
//   - 10: P1_7
// - TEST
//   -  1: P2_0
//   -  2: GND
//   -  3: P0_5
//   -  4: P0_4
//   -  5: P0_3
//   -  6: P0_2
//   -  7: P0_1
//   -  8: P0_0
//   -  9: VDD
//   - 10: P1_3
//
// We are currently using the following PINs:
// - P0_0 - Analog Input - Current sensor
// - P0_1 - Analog Input - IR1 sensor (IR1 is the door open/closed state IR sensor)
// - P0_2 - Analog Input - IR2 sensor (IR2 is the approximity sensor)
// - P0_3 - Analog Input - Battery voltage sensor
// - P0_4 - Digital Input - Lock position reed switch
// - P0_5 - Digital Output - LED
// - P1_3 - Diginal Input - Button
// - P1_4 - Timer 3 Channel 1 - PWM for Servo
// - P1_5 - Digital Output - Servo Enable
// - P1_6 - Digital Output - IR1 Enable
// - P1_7 - Digital Output - IR2 Enable
// - P2_0 - Timer 4 Channel 0 - PWM for buzzer
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "driver/ports.h"
#include "driver/timer.h"
#include "driver/buzzer.h"

// *************************************************************************************************
// Define section
#define PORTS_BUTTON_LENGTH_THRESHOLD CONV_MS_TO_TICKS(3000)

// *************************************************************************************************
// Prototype section
void ports_init(void);
void ports_button_timer(void);

// *************************************************************************************************
// Global variable section
volatile u8 ports_button_long = 0;

void ports_init(void)
{
	// set up LED
	MCU_IO_OUTPUT(1, 1, 0);		// Internal LED
	MCU_IO_OUTPUT(0, 5, 0);		// External LED
	
	// set up reed switch
	MCU_IO_INPUT(0, 4, MCU_IO_PULLDOWN);
	if (P0_4)
	{
		PICTL |= PICTL_P0ICON;		// Port 0 is looking for falling edge
	}
	else
	{
		PICTL &= ~PICTL_P0ICON;
	}
	P0IFG = 0;
	PICTL |= PICTL_P0IENH;		// Enalbe interrupt on P0_4 - P0_7
	IEN1 |= IEN1_P0IE;			// Enable interrupt at CPU level

	// set up button
	MCU_IO_INPUT(1, 3, MCU_IO_PULLDOWN);
	if (P1_3)
	{
		PICTL |= PICTL_P1ICON;
	}
	else
	{
		PICTL &= ~PICTL_P1ICON;		// Port 1 is looking for rising edge
	}
	P1IFG = 0;
	P1IEN |= BIT3;				// Enable interrupt on P1_3
	IEN2 |= IEN2_P1IE;			// Enable interrupt at CPU level
}

void ports_button_timer(void)
{
	ports_button_long = 1;
	timer1_c0_stop();
	MCU_IO_SET_LOW(0, 5);
	buzzer_off();
	buzzer_resume(); // resume buzzer status before the button press
	sys.flag.button_long = 1;
}

#pragma vector = P0INT_VECTOR
__interrupt void p0_ISR(void)
{
  	u16 i = 0;
	SLEEP &= ~SLEEP_MODE;
	P0IF = 0;
	
	if (P0IFG & BIT4)
	{
		for (i = 0; i < 0xffffu; ++i);
		if (P0_4)
		{
			sys.flag.lock = 1;
			PICTL &= ~PICTL_P0IENH;
			PICTL |= PICTL_P0ICON; // look for falling edge 
			PICTL |= PICTL_P0IENH;
		}
		else
		{
			sys.flag.unlock = 1;
			PICTL &= ~PICTL_P0IENH;
			PICTL &= ~PICTL_P0ICON; // look for rising edge 
			PICTL |= PICTL_P0IENH;
		}
	}
	P0IFG = 0;
}


#pragma vector = P1INT_VECTOR
__interrupt void p1_ISR(void)
{
	u16 i = 0;
	SLEEP &= ~SLEEP_MODE;
	P1IF = 0;
	
	if (P1IFG & BIT3)
	{
		for (i = 0; i < 0xffffu; ++i);
		if (P1_3)
		{
			PICTL |= PICTL_P1ICON; // look for falling edge
			MCU_IO_SET_HIGH(0, 5);
			buzzer_pause(); // make sure we don't have timer conflicts
			buzzer_off();
			ports_button_long = 0;
			timer1_c0_start(CONV_MS_TO_TICKS(3000), ports_button_timer);
		}
		else
		{
			PICTL &= ~PICTL_P1ICON; // look for rising edge
			if (!ports_button_long)
			{
				timer1_c0_stop();
				MCU_IO_SET_LOW(0, 5);
				//buzzer_off();
				buzzer_resume(); // resume buzzer status before the button press
				sys.flag.button = 1;
			}
		}
	}
	P1IFG = 0;
	P1IF = 0;
}

#pragma vector = P2INT_VECTOR
__interrupt void p2_ISR(void)
{
	SLEEP &= ~SLEEP_MODE;
	P2IF = 0;
	P2IFG = 0;
}

