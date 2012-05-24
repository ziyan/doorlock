// *************************************************************************************************
// 
// *************************************************************************************************

#ifndef _DRIVER_SERVO_H_
#define _DRIVER_SERVO_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Define section
#define SERVO_POSITION_LOCKED	(-15)
#define SERVO_POSITION_UNLOCKED	(100)

// *************************************************************************************************
// Prototype section
extern void servo_init(void);
extern void servo_enable(s8 position);
extern void servo_disable(void);
extern u8 servo_is_stalled(void);

#endif /*_DRIVER_SERVO_H_*/

