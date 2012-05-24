// *************************************************************************************************
// Buzzer functions.
// *************************************************************************************************

#ifndef _DRIVER_BUZZER_H_
#define _DRIVER_BUZZER_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Define section
#define BUZZER_OPTION_TOGGLE_LED	(1u << 0)

// *************************************************************************************************
// Prototype section
extern void buzzer_start(u8 repeat, u16 on_ticks, u16 off_ticks, u8 options);
extern void buzzer_stop(void);
extern void buzzer_on(void);
extern void buzzer_off(void);
extern void buzzer_pause(void);
extern void buzzer_resume(void);

#endif /*_DRIVER_BUZZER_H_*/

