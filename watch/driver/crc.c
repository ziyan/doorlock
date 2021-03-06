// *************************************************************************************************
// Calculate crc-16 checksum
// *************************************************************************************************

// *************************************************************************************************
// Include section

#include "project.h"

#include "driver/crc.h"

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Prototypes section

u16 crc_compute(u8 *data, u16 length);


// *************************************************************************************************
// @fn          crc16_compute
// @brief       compute crc-16 checksum for provided data
// @param       data, length
// @return      crc16 checksum
// *************************************************************************************************
u16 crc_compute(u8 *data, u16 length)
{
	u8 i;
	
	// initialize
	CRCINIRES = 0xffff;
	
	for (i = 0; i < length; i++)
	{
		CRCDI_L = *(data++);
	}
	return CRCINIRES;
}
