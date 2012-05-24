// *************************************************************************************************
//
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "driver/ir.h"
#include "driver/adc.h"
#include "driver/timer.h"

// *************************************************************************************************
// Define section
#define IR_SETTLE_TIME 75

// *************************************************************************************************
// Prototype section
void ir_init(void);
s8 ir_sample(u8 ir);

void ir_init(void)
{
	// set up ir enable
	MCU_IO_OUTPUT(1, 6, 0);		// IR1 Enable
	MCU_IO_OUTPUT(1, 7, 0);		// IR2 Enable
}

s8 ir_sample(u8 ir)
{
	s16 value = 0;
	if (ir == IR_DOOR)
	{
		MCU_IO_SET_HIGH(1, 6);
		timer2_delay(IR_SETTLE_TIME);
		value = adc_sample_single(ADC_REF_1_25_V, ADC_7_BIT, ADC_AIN1);
		MCU_IO_SET_LOW(1, 6);
	}
	else if (ir == IR_APPROACH)
	{
		MCU_IO_SET_HIGH(1, 7);
		timer2_delay(IR_SETTLE_TIME);
		value = adc_sample_single(ADC_REF_1_25_V, ADC_7_BIT, ADC_AIN2);
		MCU_IO_SET_LOW(1, 7);
	}
	return (value >> 8) & 0xFF;
}

