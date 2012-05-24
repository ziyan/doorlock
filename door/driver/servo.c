// *************************************************************************************************
//
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "driver/servo.h"
#include "driver/timer.h"
#include "driver/adc.h"

// *************************************************************************************************
// Prototype section
#define SERVO_CURRENT_STALL_THRESHOLD 		(0x2000)
#define SERVO_CURRENT_AVG_WINDOW_SIZE		(3)

// *************************************************************************************************
// Prototype section
void servo_init(void);
void servo_enable(s8 position);
void servo_disable(void);
u8 servo_is_stalled(void);

// *************************************************************************************************
// Global variables section
s16 servo_current_avg = 0;

void servo_init(void)
{
	// set up servo enable
	MCU_IO_OUTPUT(1, 5, 0);
}

void servo_enable(s8 position)
{
	servo_current_avg = 0;
	// generate PWM
	timer3_pwm_enable(position);
	// enable servo
	MCU_IO_SET_HIGH(1, 5);
}

void servo_disable()
{
	// disable servo
	MCU_IO_SET_LOW(1, 5);
	// stop PWM
	timer3_pwm_disable();
}

u8 servo_is_stalled(void)
{
	s16 value = adc_sample_single(ADC_REF_1_25_V, ADC_10_BIT, ADC_AIN0);
	servo_current_avg *= SERVO_CURRENT_AVG_WINDOW_SIZE - 1;
	servo_current_avg += value;
	servo_current_avg /= SERVO_CURRENT_AVG_WINDOW_SIZE;
	return (servo_current_avg > SERVO_CURRENT_STALL_THRESHOLD);
}

