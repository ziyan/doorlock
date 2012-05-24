// *************************************************************************************************
//
// Actual revision: $Revision: $
// Revision label:  $Name: $
// Revision state:  $State: $
//
// *************************************************************************************************
// Radio core access functions.
// *************************************************************************************************

#ifndef _DRIVER_RF1A_H_
#define _DRIVER_RF1A_H_

// *************************************************************************************************
// Include section
#include "project.h"


// *************************************************************************************************
// Prototype section
u8 rf1a_strobe(u8 strobe);

u8 rf1a_write_single(u8 addr, u8 value);

u8 rf1a_read_rx(u8 *data, u8 length);
u8 rf1a_write_tx(u8 *data, u8 length);

u8 rf1a_write_patable(u8 patable[8]);

#endif /*_DRIVER_RF1A_H_*/
