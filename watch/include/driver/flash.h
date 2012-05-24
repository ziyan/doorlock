// *************************************************************************************************
// Flash segment write and erase functions.
// *************************************************************************************************

#ifndef _DRIVER_FLASH_H_
#define _DRIVER_FLASH_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Defines section
#define FLASH_INFO_SEG_SIZE (128u)
#define FLASH_INFO_SEG_A_ADDR (0x1980)
#define FLASH_INFO_SEG_B_ADDR (0x1900)
#define FLASH_INFO_SEG_C_ADDR (0x1880)
#define FLASH_INFO_SEG_D_ADDR (0x1800)

// *************************************************************************************************
// Prototypes section

extern void flash_erase_segment(u8 *addr);
extern void flash_write_segment(u8 *addr, u8 data[FLASH_INFO_SEG_SIZE]);

// *************************************************************************************************
// Global Variable section



// *************************************************************************************************
// Extern section

#endif /*_DRIVER_FLASH_H_*/
