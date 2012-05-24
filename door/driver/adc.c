// *************************************************************************************************
// ADC functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section

#include "driver/adc.h"

// *************************************************************************************************
// Define section

// Macro for setting up a single conversion. If ADCCON1.STSEL = 11, using this
// macro will also start the conversion.
#define ADC_SINGLE_CONVERSION(settings) st( ADCCON3 = (settings); )

// Macro for setting up a single conversion
#define ADC_SEQUENCE_SETUP(settings)    st( ADCCON2 = (settings); )

// Macro for starting the ADC in continuous conversion mode
#define ADC_SAMPLE_CONTINUOUS()   \
  st( ADCCON1 &= ~0x30;           \
      ADCCON1 |= 0x10; )

// Macro for stopping the ADC in continuous mode
#define ADC_STOP()  st( ADCCON1 |= 0x30; )

// Macro for initiating a single sample in single-conversion mode (ADCCON1.STSEL = 11).
#define ADC_SAMPLE_SINGLE()       \
  st( ADC_STOP();                 \
      ADCCON1 |= 0x40; )

// Macro for configuring the ADC to be started from T1 channel 0. (T1 ch 0 must be in compare mode!!)
#define ADC_TRIGGER_FROM_TIMER1() \
  st( ADC_STOP();                 \
      ADCCON1 &= ~0x10; )

// Expression indicating whether a conversion is finished or not.
#define ADC_SAMPLE_READY()      st( ADCCON1 & 0x80; )

// Macro for setting/clearing a channel as input of the ADC
#define ADC_ENABLE_CHANNEL(ch)   ADCCFG |=  (0x01 << ch)
#define ADC_DISABLE_CHANNEL(ch)  ADCCFG &= ~(0x01 << ch)


// Macros for configuring the ADC input:
// Example usage:
//   IO_ADC_PORT0_PIN(0, IO_ADC_EN);
//   IO_ADC_PORT0_PIN(4, IO_ADC_DIS);
//   IO_ADC_PORT0_PIN(6, IO_ADC_EN);

#define IO_ADC_PORT0_PIN(pin, adcEn)    \
  st( if (adcEn)                        \
        ADCCFG |= BM( pin );            \
      else                              \
        ADCCFG &= ~BM( pin ); )

// where adcEn is one of:
#define IO_ADC_EN           1 // ADC input enabled
#define IO_ADC_DIS          0 // ADC input disab


// *************************************************************************************************
// Prototype section
void adc_init(void);
s16 adc_sample_single(u8 reference, u8 resolution, u8 channel);

void adc_init(void)
{
}

// *************************************************************************************************
// @fn		adc_sample_single
// @brief	This function makes the adc sample the given channel at the given resolution with
// 			the given reference.
// @param	u8 reference - The reference to compare the channel to be sampled.
// 			u8 resolution - The resolution to use during the sample (8, 10, 12 or 14 bit)
// 			u8 input - The channel to be sampled.
// @return	s16 - The conversion result
// *************************************************************************************************
s16 adc_sample_single(u8 reference, u8 resolution, u8 channel)
{
    s16 value;

    ADC_ENABLE_CHANNEL(channel);

    ADCIF = 0;
    ADC_SINGLE_CONVERSION(reference | resolution | channel);
    while(!ADCIF);

    value  = (ADCH << 8) & 0xff00;
    value |= ADCL;

    ADC_DISABLE_CHANNEL(channel);

    //  The variable 'value' contains 16 bits where
    //     bit 15 is a sign bit
    //     bit [14 .. 0] contain the absolute sample value
    //     Only the r upper bits are significant, where r is the resolution
    //     Resolution:
    //        12   -> [14 .. 3] (bitmask 0x7FF8)
    //        10   -> [14 .. 5] (bitmask 0x7FE0)
    //         9   -> [14 .. 6] (bitmask 0x7FC0)
    //         7   -> [14 .. 8] (bitmask 0x7F00)

    return value;
}

// *************************************************************************************************
// Copyright 2007 Texas Instruments Incorporated. All rights reserved.
//
// IMPORTANT: Your use of this Software is limited to those specific rights
// granted under the terms of a software license agreement between the user
// who downloaded the software, his/her employer (which must be your employer)
// and Texas Instruments Incorporated (the "License").  You may not use this
// Software unless you agree to abide by the terms of the License. The License
// limits your use, and you acknowledge, that the Software may not be modified,
// copied or distributed unless embedded on a Texas Instruments microcontroller
// or used solely and exclusively in conjunction with a Texas Instruments radio
// frequency transceiver, which is integrated into your product.  Other than for
// the foregoing purpose, you may not use, reproduce, copy, prepare derivative
// works of, modify, distribute, perform, display or sell this Software and/or
// its documentation for any purpose.
//
// YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
// PROVIDED “AS IS” WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
// INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
// NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
// TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
// NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
// LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
// INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
// OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
// OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
// (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
//
// Should you have any questions regarding your right to use this Software,
// contact Texas Instruments Incorporated at www.TI.com.
// *************************************************************************************************
