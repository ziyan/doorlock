// *************************************************************************************************
// ADC functions.
// *************************************************************************************************

#ifndef _DRIVER_ADC_H_
#define _DRIVER_ADC_H_

// *************************************************************************************************
// Include section
#include "project.h"

// *************************************************************************************************
// Define section

// Reference voltage:
#define ADC_REF_1_25_V      0x00     // Internal 1.25V reference
#define ADC_REF_P0_7        0x40     // External reference on AIN7 pin
#define ADC_REF_AVDD        0x80     // AVDD_SOC pin
#define ADC_REF_P0_6_P0_7   0xC0     // External reference on AIN6-AIN7 differential input

// Resolution (decimation rate):
#define ADC_7_BIT           0x00     //  64 decimation rate
#define ADC_9_BIT           0x10     // 128 decimation rate
#define ADC_10_BIT          0x20     // 256 decimation rate
#define ADC_12_BIT          0x30     // 512 decimation rate

// Input channel:
#define ADC_AIN0            0x00     // single ended P0_0
#define ADC_AIN1            0x01     // single ended P0_1
#define ADC_AIN2            0x02     // single ended P0_2
#define ADC_AIN3            0x03     // single ended P0_3
#define ADC_AIN4            0x04     // single ended P0_4
#define ADC_AIN5            0x05     // single ended P0_5
#define ADC_AIN6            0x06     // single ended P0_6
#define ADC_AIN7            0x07     // single ended P0_7
#define ADC_GND             0x0C     // Ground
#define ADC_VDD				0x0D     // Positive voltage reference
#define ADC_TEMP_SENS       0x0E     // on-chip temperature sensor
#define ADC_VDD_3           0x0F     // (vdd/3)


// *************************************************************************************************
// Prototype section

extern void adc_init(void);
extern s16 adc_sample_single(u8 reference, u8 resolution, u8 channel);

#endif /*_DRIVER_ADC_H_*/
