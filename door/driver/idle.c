// *************************************************************************************************
// Sleep mode code.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "driver/idle.h"

#define ST_EVENT0 (6400u)

// *************************************************************************************************
// Prototype section
void idle_init(void);
void idle_pm0(void);
void idle_pm2(void);


void idle_init(void)
{
	u8 temp;

	// Reset timer, update EVENT0
	WORCTL &= ~WORCTL_WOR_RES;
	WORCTL |= WORCTL_WOR_RES_1;
	
	WORCTL |= WORCTL_WOR_RESET;				// Reset Sleep Timer
	temp = WORTIME0;
	while(temp == WORTIME0);				// Wait until a positive 32 kHz edge
	temp = WORTIME0;
	while(temp == WORTIME0);				// Wait until a positive 32 kHz edge
	WOREVT1 = ST_EVENT0 >> 8;				// Set EVENT0, high byte
	WOREVT0 = ST_EVENT0 & 0xff;				// Set EVENT0, low byte
}

// *************************************************************************************************
// @fn          idle_pm0
// @brief       Go to sleep mode
// @param       none
// @return      none
// *************************************************************************************************
void idle_pm0(void)
{
	// go to PM0
	PCON |= PCON_IDLE;
}


// *************************************************************************************************
// @fn          idle_pm2
// @brief       Go to sleep mode
// @param       none
// @return      none
// *************************************************************************************************
void idle_pm2(void)
{
	u8 temp;

	// prepare to go to deep sleep
	T1CTL &= ~T1CTL_MODE;

	// If timer is not in use, set sleep timer and goto PM2
	// going to PM2
	SLEEP |= SLEEP_MODE_PM2;

	// Enable sleep timer interrupt
	WORIRQ |= WORIRQ_EVENT0_MASK;
	STIE = 1;
	
	// Reset timer, and enter PM{0 – 2}
	WORCTL |= WORCTL_WOR_RESET;				// Reset Sleep Timer
	temp = WORTIME0;
	while(temp == WORTIME0);				// Wait until a positive 32 kHz edge
	temp = WORTIME0;
	while(temp == WORTIME0);				// Wait until a positive 32 kHz edge
	PCON |= PCON_IDLE;						// Enter PM{0 – 2}

	T1CTL |= T1CTL_MODE_FREERUN;
}


#pragma vector = ST_VECTOR
__interrupt void sleep_timer_ISR(void)
{
	// clear sleep mode
	SLEEP &= ~SLEEP_MODE;
	
	// disable sleep timer interrupt
	STIE = 0;
	WORIRQ &= ~WORIRQ_EVENT0_MASK;
	
	// clear cpu interrupt flag
	STIF = 0;

	if (WORIRQ & WORIRQ_EVENT0_FLAG)
	{
		WORIRQ &= ~WORIRQ_EVENT0_FLAG;
		sys.flag.sleep_wakeup = 1;
	}
}

