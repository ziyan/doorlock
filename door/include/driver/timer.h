// *************************************************************************************************
// Timer functions.
// - timer1_c0 - used by button/buzzer
// - timer1_c1 - used by radio
// - timer1_c2 - used by lock
// *************************************************************************************************

#ifndef _DRIVER_TIMER_H_
#define _DRIVER_TIMER_H_

// *************************************************************************************************
// Include section

#include "project.h"

// *************************************************************************************************
// Define section
#define CONV_US_TO_TICKS(usec) ((u32)usec * 5859 / 1000000)
#define CONV_MS_TO_TICKS(msec) ((u32)msec * 5859 / 1000)

// *************************************************************************************************
// Prototype section

extern void timer_init(void);
extern u8 timer_running(void);

extern void timer1_c0_start(u16 ticks, void (*callback)(void));
extern void timer1_c0_stop(void);
extern void timer1_c1_start(u16 ticks, void (*callback)(void));
extern void timer1_c1_stop(void);
extern void timer1_c2_start(u16 ticks, void (*callback)(void));
extern void timer1_c2_stop(void);

extern void timer2_delay(u16 msec);

extern void timer3_pwm_enable(s8 value);
extern void timer3_pwm_disable(void);
extern void timer4_pwm_enable(u8 value);
extern void timer4_pwm_disable(void);

#endif /*_DRIVER_TIMER_H_*/

