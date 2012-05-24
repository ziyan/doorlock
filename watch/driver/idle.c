// *************************************************************************************************
// Idle loop
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "project.h"

#include "driver/idle.h"

// *************************************************************************************************
// Prototypes section
void idle(void);
void to_lpm(void);

// *************************************************************************************************
// Defines section


// *************************************************************************************************
// Global Variable section


// *************************************************************************************************
// Extern section


void idle(void)
{
	// To low power mode
	to_lpm();

#ifdef USE_WATCHDOG		
	// Service watchdog
	WDTCTL = WDTPW + WDTIS__512K + WDTSSEL__ACLK + WDTCNTCL;
#endif
}

// *************************************************************************************************
// @fn          to_lpm
// @brief       Go to LPM0/3. 
// @param       none
// @return      none
// *************************************************************************************************
void to_lpm(void)
{
#ifdef CC430_IS_REV_B
	u8 state;

	// Go to LPM3 whenever radio is IDLE
	// Go to LPM3 when radio is in some RX or TX mode
	state = rf1a_strobe(RF_SNOP);
	
	if ((state & 0x70) == 0)
	{
		// radio in IDLE state
		_BIS_SR(LPM3_bits + GIE); 
		__no_operation();	
	}
	else
	{
		// radio is active
		_BIS_SR(LPM0_bits + GIE); 
		__no_operation();	
	}
#endif

#ifdef CC430_IS_REV_C
	// Go to LPM3
	_BIS_SR(LPM3_bits + GIE); 
	__no_operation();
#endif
}

