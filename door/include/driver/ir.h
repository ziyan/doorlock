// *************************************************************************************************
// IR driver
// *************************************************************************************************

#ifndef _DRIVER_IR_H_
#define _DRIVER_IR_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Define section
#define IR_DOOR		(0u)
#define IR_APPROACH	(1u)

// *************************************************************************************************
// Prototype section
extern void ir_init(void);
extern s8 ir_sample(u8 ir);

#endif /*_DRIVER_IR_H_*/

