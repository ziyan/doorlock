// *************************************************************************************************
// Flash segment write and erase functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "project.h"

#include "driver/flash.h"

// *************************************************************************************************
// Prototypes section
void flash_erase_segment(u8 *addr);
void flash_write_segment(u8 *addr, u8 data[FLASH_INFO_SEG_SIZE]);

/* Internal use */
void flash_unlock(void);
void flash_lock(void);

// *************************************************************************************************
// Global Variable section

// *************************************************************************************************
// Defines section
#define WDTPW_XOR 0x3300

// *************************************************************************************************
// Extern section

// *************************************************************************************************
// @fn          flash_erase_segment
// @brief       Erase flash segment
// @param       addr - Start address of the segment
// @return      status
// *************************************************************************************************
void flash_erase_segment(u8 *addr)
{
	flash_unlock();
	
	// Enable segment erase (results in MERAS = 0, ERASE = 1)
	FCTL1 = FWKEY | ERASE;

	// Dummy write to initiate flash erase in segment A, wait for completion
	*addr = 0;	
	while(FCTL3 & BUSY);
	
	flash_lock();
}

// *************************************************************************************************
// @fn          flash_write_segment
// @brief       Write flash segment
// @param       addr - start address of the segment, data
// @return      status
// *************************************************************************************************
void flash_write_segment(u8 *addr, u8 data[FLASH_INFO_SEG_SIZE])
{
	u8 i;
	
	flash_unlock();
	
	FCTL1 = FWKEY | WRT;

	// write to Flash
	for(i = 0; i < FLASH_INFO_SEG_SIZE; addr++, i++)
	{
		*addr = data[i];
		while (FCTL3 & BUSY);
	}
	
	FCTL1 = FWKEY;

	flash_lock();
}

// *************************************************************************************************
// @fn          flash_unlock
// @brief       internal use
// @param       none
// @return      none
// *************************************************************************************************
void flash_unlock(void)
{
	// Disable WDT (without disturbing the rest of the register)
	WDTCTL = (WDTCTL ^ WDTPW_XOR) & ~WDTHOLD;
	
	// Wait for any flash operations to complete
	while (FCTL3 & BUSY);
	
	// Unlock Flash
	FCTL3 = FWKEY;
	
	// Unlock information bank  (clears all lower byte => clears LOCKINFO)
	FCTL4 = FWKEY;
	
	if(FCTL3 & LOCKA)
	{
		// Segment A of the info bank is locked, indicating
		// all segments are locked. Write a 1 to toggle its state
		FCTL3 = FWKEY | LOCKA;
	}
}

// *************************************************************************************************
// @fn          flash_lock
// @brief       internal use
// @param       none
// @return      none
// *************************************************************************************************
void flash_lock(void)
{
	while (FCTL3 & BUSY);
	
	// Lock segment A
	if(!(FCTL3 & LOCKA))
	{
		// Segment A of the info bank is unlocked. Lock it now
		FCTL3 = FWKEY | LOCKA;
	}
	
	// Lock the Info bank
	FCTL4 = FWKEY | LOCKINFO;

	// Lock Flash
	FCTL3 = FWKEY | LOCK;
	
	// Re-enable WDT
#ifdef USE_WATCHDOG	
	WDTCTL = (WDTCTL ^ WDTPW_XOR) | WDTHOLD;
#endif
}


