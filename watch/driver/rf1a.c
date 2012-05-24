// *************************************************************************************************
//
// Actual revision: $Revision: $
// Revision label:  $Name: $
// Revision state:  $State: $
//
// *************************************************************************************************
// Radio core access functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section

// system
#include "project.h"

// driver
#include "driver/rf1a.h"


// *************************************************************************************************
// Global section


// *************************************************************************************************
// Define section
#define st(x)      					do { x } while (__LINE__ == -1)
#define ENTER_CRITICAL_SECTION(x)  	st( x = __get_interrupt_state(); __disable_interrupt(); )
#define EXIT_CRITICAL_SECTION(x)    __set_interrupt_state(x)

// *************************************************************************************************
// @fn          rf1a_strobe
// @brief       Send command to radio.
// @param       none
// @return      none
// *************************************************************************************************
u8 rf1a_strobe(u8 strobe)
{
	u16 int_state = 0;
	u8 status = 0;
	
	// first check if strobe is a command
	if ((strobe & 0x7f) < RF_SRES || (strobe & 0x7f) > RF_SNOP) return 0;
	
	ENTER_CRITICAL_SECTION(int_state);
	
	// clear the stat ready flag
	RF1AIFCTL1 &= ~RFSTATIFG;
	RF1AIFERR = 0;
	
	// wait till radio accepts the next instruction
	while (!(RF1AIFCTL1 & RFINSTRIFG));
	
	// send instruction
	RF1AINSTRB = strobe;
	
	// SRES command does not return any status
	if ((strobe & 0x7f) == RF_SRES) return 0;
	
	// wait till radio status is ready
	while (!((RF1AIFCTL1 & RFSTATIFG) || (RF1AIFCTL1 & RFERRIFG)));
	
	if (RF1AIFCTL1 & RFERRIFG)
	{
		RF1AIFERR = 0;
	}
	else
	{
		// read status
		status = RF1ASTATB;
	}
	
	EXIT_CRITICAL_SECTION(int_state);

	return status;
}

// *************************************************************************************************
// @fn          rf1a_write_single
// @brief       Write byte to register.
// @param       none
// @return      status
// *************************************************************************************************
u8 rf1a_write_single(u8 addr, u8 value)
{
	u16 int_state = 0;
	u8 status = 0;
	
	// check to see if register within range
	if ((addr & 0x3f) > TEST0) return 0;
	
	ENTER_CRITICAL_SECTION(int_state);
	
	// clear the stat ready flag
	RF1AIFCTL1 &= ~RFSTATIFG;
	RF1AIFERR = 0;
	
	// wait till radio accepts next instruction
	while (!(RF1AIFCTL1 & RFINSTRIFG));
	
	// send address of register
	RF1AINSTRB = ((addr & 0x3f) | RF_SNGLREGWR);
	
	// wait till radio status is ready
	while (!((RF1AIFCTL1 & RFSTATIFG) || (RF1AIFCTL1 & RFERRIFG)));
	
	if (RF1AIFCTL1 & RFERRIFG)
	{
		RF1AIFERR = 0;
	}
	else
	{
		// read status
		status = RF1ASTATB;
		
		// wait till ready for next data
		while (!(RF1AIFCTL1 & RFDINIFG));
		
		// send data
		RF1ADINB = value;
	}

	EXIT_CRITICAL_SECTION(int_state);
	
	return status;
}


// *************************************************************************************************
// @fn          rf1a_write_burst
// @brief       Write bytes to register.
// @param       none
// @return      status
// *************************************************************************************************
u8 rf1a_write_burst(u8 addr, u8 *values, u8 length)
{
	u16 int_state = 0;
	u8 status = 0;
	u8 i = 0;
	
	// check to see if register within range
	if (addr > TEST0) return 0;
	
	ENTER_CRITICAL_SECTION(int_state);
	
	// clear the stat ready flag
	RF1AIFCTL1 &= ~RFSTATIFG;
	RF1AIFERR = 0;
	
	// wait till radio accepts next instruction
	while (!(RF1AIFCTL1 & RFINSTRIFG));
	
	// send address of register
	RF1AINSTRB = (addr | RF_REGWR);
	
	// wait till radio status is ready
	while (!((RF1AIFCTL1 & RFSTATIFG) || (RF1AIFCTL1 & RFERRIFG)));
	
	if (RF1AIFCTL1 & RFERRIFG)
	{
		RF1AIFERR = 0;
	}
	else
	{
		// read status
		status = RF1ASTATB;
	
		for (i = 0; i < length; i++)
		{
			// wait till ready for next data
			while (!(RF1AIFCTL1 & RFDINIFG));
			RF1ADINB = values[i];
		}
	}
	
	EXIT_CRITICAL_SECTION(int_state);
	
	return status;
}


// *************************************************************************************************
// @fn          rf1a_read_rx
// @brief       Read sequence of bytes from register.
// @param       none
// @return      none
// *************************************************************************************************
u8 rf1a_read_rx(u8 *data, u8 length)
{
	u16 int_state = 0;
	u8 status = 0, i = 0;
	
	ENTER_CRITICAL_SECTION(int_state);
	
	// clears status ready
	RF1AIFCTL1 &= ~RFSTATIFG;
	RF1AIFERR = 0;
	
	// wait till ready for new instruction
	while(!(RF1AIFCTL1 & RFINSTRIFG));
	
	// write read rx command (auto read)
	RF1AINSTR1B = RF_RXFIFORD;
	
	// wait till radio status is ready
	while (!((RF1AIFCTL1 & RFSTATIFG) || (RF1AIFCTL1 & RFERRIFG)));
	
	if (RF1AIFCTL1 & RFERRIFG)
	{
		RF1AIFERR = 0;
	}
	else
	{
		// read status
		status = RF1ASTATB;
		
		for (i = 0; i < length - 1; i++)
		{
			// wait till data is ready
			while (!(RF1AIFCTL1 & RFDOUTIFG));
			
			// read data (auto read)
			data[i] = RF1ADOUT1B;
		}
		
		// wait till data is ready
		while (!(RF1AIFCTL1 & RFDOUTIFG));
		
		data[length - 1] = RF1ADOUTB;
	}
	
	EXIT_CRITICAL_SECTION(int_state);
	
	return status;
}  


// *************************************************************************************************
// @fn          rf1a_write_tx
// @brief       Write sequence of bytes to register.
// @param       none
// @return      none
// *************************************************************************************************
u8 rf1a_write_tx(u8 *data, u8 length)
{
	u16 int_state = 0;
	u8 status = 0, i = 0;
	
	ENTER_CRITICAL_SECTION(int_state);
	
	// clears status ready
	RF1AIFCTL1 &= ~RFSTATIFG;
	RF1AIFERR = 0;
	
	// wait till ready for new instruction
	while (!(RF1AIFCTL1 & RFINSTRIFG));
	
	// write tx command
	RF1AINSTRB = RF_TXFIFOWR;
	
	// wait till radio status is ready
	while (!((RF1AIFCTL1 & RFSTATIFG) || (RF1AIFCTL1 & RFERRIFG)));
	
	if (RF1AIFCTL1 & RFERRIFG)
	{
		RF1AIFERR = 0;
	}
	else
	{
		// read status
		status = RF1ASTATB;
		
		for (i = 0; i < length; i++)
		{
			// wait till ready for new data
			while (!(RF1AIFCTL1 & RFDINIFG));
			
			// write data
			RF1ADINB = data[i];
		}
	}
	
	EXIT_CRITICAL_SECTION(int_state);

	return status;
}

u8 rf1a_write_patable(u8 patable[8])
{
	u16 int_state = 0;
	u8 i = 0, status = 0;
	
	ENTER_CRITICAL_SECTION(int_state);
	
	// clears status ready
	RF1AIFCTL1 &= ~RFSTATIFG;
	RF1AIFERR = 0;
	
	// wait for ready instruction
	while(!(RF1AIFCTL1 & RFINSTRIFG));
	
	// write power table write instruction
	RF1AINSTRB = RF_PATABWR;
	
	// wait till radio status is ready
	while (!((RF1AIFCTL1 & RFSTATIFG) || (RF1AIFCTL1 & RFERRIFG)));
	
	if (RF1AIFCTL1 & RFERRIFG)
	{
		RF1AIFERR = 0;
	}
	else
	{
		// read status
		status = RF1ASTATB;
	
		for (i = 0; i < 8; ++i)
		{
			// wait till ready for new data
			while (!(RF1AIFCTL1 & RFDINIFG));
			
			// write power table
			RF1ADINB = patable[i];
		}
	}
	
	EXIT_CRITICAL_SECTION(int_state);

	return status;
}


