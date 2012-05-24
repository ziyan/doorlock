// *************************************************************************************************
//
//	Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/ 
//	 
//	 
//	  Redistribution and use in source and binary forms, with or without 
//	  modification, are permitted provided that the following conditions 
//	  are met:
//	
//	    Redistributions of source code must retain the above copyright 
//	    notice, this list of conditions and the following disclaimer.
//	 
//	    Redistributions in binary form must reproduce the above copyright
//	    notice, this list of conditions and the following disclaimer in the 
//	    documentation and/or other materials provided with the   
//	    distribution.
//	 
//	    Neither the name of Texas Instruments Incorporated nor the names of
//	    its contributors may be used to endorse or promote products derived
//	    from this software without specific prior written permission.
//	
//	  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//	  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//	  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//	  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//	  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//	  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//	  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//	  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//	  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//	  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//	  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
// Radio core access functions. Taken from TI reference code for CC430.
// *************************************************************************************************

// system
#include "project.h"

// driver
#include "driver/radio.h"
#include "driver/rf1a.h"
#include "driver/display.h"
#include "driver/smartrf.h"

void radio_config(void)
{	
	u8 patable[8] =
	{
		SMARTRF_SETTING_PATABLE0,
		SMARTRF_SETTING_PATABLE1,
		SMARTRF_SETTING_PATABLE2,
		SMARTRF_SETTING_PATABLE3,
		SMARTRF_SETTING_PATABLE4,
		SMARTRF_SETTING_PATABLE5,
		SMARTRF_SETTING_PATABLE6,
		SMARTRF_SETTING_PATABLE7
	};
	
	/*u8 config[47]=
	{
		SMARTRF_SETTING_IOCFG2  , // IOCFG2: GDO2 signals on RF_RDYn     
		SMARTRF_SETTING_IOCFG1  , // IOCFG1: GDO1 signals on RSSI_VALID     
		SMARTRF_SETTING_IOCFG0  , // IOCFG0: GDO0 signals on PA power down signal to control RX/TX switch         
		SMARTRF_SETTING_FIFOTHR , // FIFOTHR: RX/TX FIFO Threshold: 33 bytes in TX, 32 bytes in RX    
		0xD3                    , // SYNC1: high byte of Sync Word
		0x91                    , // SYNC0: low byte of Sync Word
		SMARTRF_SETTING_PKTLEN  , // PKTLEN: Packet Length in fixed mode, Maximum Length in variable-length mode      
		SMARTRF_SETTING_PKTCTRL1, // PKTCTRL1: No status bytes appended to the packet    
		SMARTRF_SETTING_PKTCTRL0, // PKTCTRL0: Fixed-Length Mode, No CRC       
		SMARTRF_SETTING_ADDR    , // ADDR: Address for packet filtration       
		SMARTRF_SETTING_CHANNR  , // CHANNR: 8-bit channel number, freq = base freq + CHANNR * channel spacing          
		SMARTRF_SETTING_FSCTRL1 , // FSCTRL1: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio) 
		SMARTRF_SETTING_FSCTRL0 , // FSCTRL0: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio) 
		SMARTRF_SETTING_FREQ2   , // FREQ2: base frequency, high byte      
		SMARTRF_SETTING_FREQ1   , // FREQ1: base frequency, middle byte      
		SMARTRF_SETTING_FREQ0   , // FREQ0: base frequency, low byte      
		SMARTRF_SETTING_MDMCFG4 , // MDMCFG4: modem configuration (refer to User's Guide/SmartRF Studio)     
		SMARTRF_SETTING_MDMCFG3 , // MDMCFG3:                "                      "    
		SMARTRF_SETTING_MDMCFG2 , // MDMCFG2:                "                      "        
		SMARTRF_SETTING_MDMCFG1 , // MDMCFG1:                "                      "        
		SMARTRF_SETTING_MDMCFG0 , // MDMCFG0:                "                      "        
		SMARTRF_SETTING_DEVIATN , // DEVIATN: modem deviation setting (refer to User's Guide/SmartRF Studio)         
		SMARTRF_SETTING_MCSM2   , // MCSM2: Main Radio Control State Machine Conf. : timeout for sync word search disabled      
		SMARTRF_SETTING_MCSM1   , // MCSM1: CCA signals when RSSI below threshold, stay in RX after packet has been received      
		SMARTRF_SETTING_MCSM0   , // MCSM0: Auto-calibrate when going from IDLE to RX or TX (or FSTXON )      
		SMARTRF_SETTING_FOCCFG  , // FOCCFG: Frequency Offset Compensation Conf.     
		SMARTRF_SETTING_BSCFG   , // BSCFG: Bit Synchronization Conf.       
		SMARTRF_SETTING_AGCCTRL2, // AGCCTRL2: AGC Control   
		SMARTRF_SETTING_AGCCTRL1, // AGCCTRL1:     "   
		SMARTRF_SETTING_AGCCTRL0, // AGCCTRL0:     "   
		SMARTRF_SETTING_WOREVT1 , // WOREVT1: High Byte Event0 Timeout    
		SMARTRF_SETTING_WOREVT0 , // WOREVT0: High Byte Event0 Timeout
		SMARTRF_SETTING_WORCTL  , // WORCTL: Wave On Radio Control ****Feature unavailable in PG0.6****
		SMARTRF_SETTING_FREND1  , // FREND1: Front End RX Conf.    
		SMARTRF_SETTING_FREND0  , // FREND0: Front End TX Conf.               
		SMARTRF_SETTING_FSCAL3  , // FSCAL3: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)    
		SMARTRF_SETTING_FSCAL2  , // FSCAL2:              "      
		SMARTRF_SETTING_FSCAL1  , // FSCAL1:              "     
		SMARTRF_SETTING_FSCAL0  , // FSCAL0:              "     
		0x00                    , // Reserved *read as 0*
		0x00                    , // Reserved *read as 0*
		SMARTRF_SETTING_FSTEST  , // FSTEST: For test only, irrelevant for normal use case
		SMARTRF_SETTING_PTEST   , // PTEST: For test only, irrelevant for normal use case
		SMARTRF_SETTING_AGCTEST , // AGCTEST: For test only, irrelevant for normal use case
		SMARTRF_SETTING_TEST2   , // TEST2  : For test only, irrelevant for normal use case    
		SMARTRF_SETTING_TEST1   , // TEST1  : For test only, irrelevant for normal use case
		SMARTRF_SETTING_TEST0     // TEST0  : For test only, irrelevant for normal use case    
	};
	
	rf1a_write_burst(IOCFG2, (u8*)config, 47);*/

	
	// IOCFG2: GDO2 signals on RF_RDYn
	rf1a_write_single(IOCFG2, SMARTRF_SETTING_IOCFG2);
	// IOCFG0: GDO0 signals on PA power down signal to control RX/TX switch     
	//rf1a_write_single(IOCFG0D, SMARTRF_SETTING_IOCFG0D);
	// IOCFG1: GDO1 signals on RSSI_VALID
	rf1a_write_single(IOCFG1, SMARTRF_SETTING_IOCFG1); 
	// IOCFG0: GDO0 signals on PA power down signal to control RX/TX switch     
	rf1a_write_single(IOCFG0, SMARTRF_SETTING_IOCFG0);
	// FIFOTHR: RX/TX FIFO Threshold: 33 bytes in TX, 32 bytes in RX         
	rf1a_write_single(FIFOTHR, SMARTRF_SETTING_FIFOTHR);
	// SYNC1: high byte of Sync Word
	rf1a_write_single(SYNC1, SMARTRF_SETTING_SYNC1); 
	// SYNC0: low byte of Sync Word
	rf1a_write_single(SYNC0, SMARTRF_SETTING_SYNC0); 
	// PKTLEN: Packet Length in fixed mode, Maximum Length in variable-length mode
	rf1a_write_single(PKTLEN, SMARTRF_SETTING_PKTLEN);
	// PKTCTRL1: status bytes appended to the packet
	rf1a_write_single(PKTCTRL1, SMARTRF_SETTING_PKTCTRL1); 
	// PKTCTRL0: Fixed-Length Mode, CRC    
	rf1a_write_single(PKTCTRL0, SMARTRF_SETTING_PKTCTRL0); 
	// ADDR: Address for packet filtration
	rf1a_write_single(ADDR, SMARTRF_SETTING_ADDR);
	// CHANNR: 8-bit channel number, freq = base freq + CHANNR * channel spacing
	rf1a_write_single(CHANNR, SMARTRF_SETTING_CHANNR);
	// FSCTRL1: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio)
	rf1a_write_single(FSCTRL1, SMARTRF_SETTING_FSCTRL1);
	// FSCTRL0: Frequency Synthesizer Control (refer to User's Guide/SmartRF Studio)
	rf1a_write_single(FSCTRL0, SMARTRF_SETTING_FSCTRL0);
	// FREQ2: base frequency, high byte
	rf1a_write_single(FREQ2, SMARTRF_SETTING_FREQ2);
	// FREQ1: base frequency, middle byte
	rf1a_write_single(FREQ1, SMARTRF_SETTING_FREQ1);
	// FREQ0: base frequency, low byte      
	rf1a_write_single(FREQ0, SMARTRF_SETTING_FREQ0);
	// MDMCFG4: modem configuration (refer to User's Guide/SmartRF Studio)   
	rf1a_write_single(MDMCFG4, SMARTRF_SETTING_MDMCFG4);
	// MDMCFG3: modem configuration (refer to User's Guide/SmartRF Studio)
	rf1a_write_single(MDMCFG3, SMARTRF_SETTING_MDMCFG3);
	// MDMCFG2: modem configuration (refer to User's Guide/SmartRF Studio)    
	rf1a_write_single(MDMCFG2, SMARTRF_SETTING_MDMCFG2);
	// MDMCFG1: modem configuration (refer to User's Guide/SmartRF Studio)      
	rf1a_write_single(MDMCFG1, SMARTRF_SETTING_MDMCFG1);
	// MDMCFG0: modem configuration (refer to User's Guide/SmartRF Studio)
	rf1a_write_single(MDMCFG0, SMARTRF_SETTING_MDMCFG0);
	// DEVIATN: modem deviation setting (refer to User's Guide/SmartRF Studio)
	rf1a_write_single(DEVIATN, SMARTRF_SETTING_DEVIATN);
	// MCSM2: Main Radio Control State Machine Conf. : timeout for sync word search disabled      
	rf1a_write_single(MCSM2, SMARTRF_SETTING_MCSM2);
	// MCSM1: CCA signals when RSSI below threshold, stay in RX after packet has been received       
	rf1a_write_single(MCSM1, SMARTRF_SETTING_MCSM1);
	// MCSM0: Auto-calibrate when going from IDLE to RX or TX (or FSTXON )
	rf1a_write_single(MCSM0, SMARTRF_SETTING_MCSM0);
	// FOCCFG: Frequency Offset Compensation Conf.
	rf1a_write_single(FOCCFG, SMARTRF_SETTING_FOCCFG);
	// BSCFG: Bit Synchronization Conf.
	rf1a_write_single(BSCFG, SMARTRF_SETTING_BSCFG);    
	// AGCCTRL2: AGC Control    
	rf1a_write_single(AGCCTRL2, SMARTRF_SETTING_AGCCTRL2);
	// AGCCTRL1: AGC Control    
	rf1a_write_single(AGCCTRL1, SMARTRF_SETTING_AGCCTRL1); 
	// AGCCTRL0: AGC Control   
	rf1a_write_single(AGCCTRL0, SMARTRF_SETTING_AGCCTRL0);
	// WOREVT1: High Byte Event0 Timeout    
	rf1a_write_single(WOREVT1, SMARTRF_SETTING_WOREVT1); 
	// WOREVT0: High Byte Event0 Timeout    
	rf1a_write_single(WOREVT0, SMARTRF_SETTING_WOREVT0); 
	// WORCTL: Wave On Radio Control ****Feature unavailable in PG0.6****
	rf1a_write_single(WORCTRL, SMARTRF_SETTING_WORCTRL); 
	// FREND1: Front End RX Conf.
	rf1a_write_single(FREND1, SMARTRF_SETTING_FREND1);
	// FREND0: Front End TX Conf.    
	rf1a_write_single(FREND0, SMARTRF_SETTING_FREND0);
	// FSCAL3: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)
	rf1a_write_single(FSCAL3, SMARTRF_SETTING_FSCAL3); 
	// FSCAL2: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)    
	rf1a_write_single(FSCAL2, SMARTRF_SETTING_FSCAL2); 
	// FSCAL1: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)      
	rf1a_write_single(FSCAL1, SMARTRF_SETTING_FSCAL1); 
	// FSCAL0: Frequency Synthesizer Calibration (refer to User's Guide/SmartRF Studio)     
	rf1a_write_single(FSCAL0, SMARTRF_SETTING_FSCAL0);
	//       
	rf1a_write_single(FSTEST, SMARTRF_SETTING_FSTEST);
	//
	rf1a_write_single(TEST2, SMARTRF_SETTING_TEST2);
	//
	rf1a_write_single(TEST1, SMARTRF_SETTING_TEST1);
	//
	rf1a_write_single(TEST0, SMARTRF_SETTING_TEST0);
	
	rf1a_write_patable(patable);

}


// *************************************************************************************************
// @fn          radio_reset
// @brief       Reset radio core. 
// @param       none
// @return      none
// *************************************************************************************************
void radio_reset(void)
{
	volatile u16 i;
	u8 x;
	
	// Reset radio core
	rf1a_strobe(RF_SRES);
	
	// Wait before checking IDLE 
	for (i=0; i<1000; i++);
	do {
		x = rf1a_strobe(RF_SIDLE);
	} while (x & 0xF0);
	
	// Clear radio error register
	RF1AIFERR = 0;
}


// *************************************************************************************************
// @fn          radio_powerdown
// @brief       Put radio to SLEEP mode. 
// @param       none
// @return      none
// *************************************************************************************************
void radio_powerdown(void)
{
	u8 status = 0;
	// Powerdown radio
	status = rf1a_strobe(RF_SIDLE);
	while (status & 0xF0)
	{
		status = rf1a_strobe(RF_SNOP);
	} 
	rf1a_strobe(RF_SXOFF);
}

void radio_receive_on(void)
{
	u8 status = 0;
	
	// get radio into idle state
	status = rf1a_strobe(RF_SIDLE);
	while (status & 0xF0)
	{
		status = rf1a_strobe(RF_SNOP);
	}
	
	// clear rx fifo
	status = rf1a_strobe(RF_SFRX | RF_RXSTAT);
	while (status & 0x0F)
	{
		status = rf1a_strobe(RF_SNOP | RF_RXSTAT);
	}

	// turn on receive interrupt
	RF1AIFG &= ~BITA;
  	RF1AIE  |= BITA;

	// get into rx mode
	status = rf1a_strobe(RF_SRX);
	while ((status & 0xF0) != 0x10)
	{
		status = rf1a_strobe(RF_SNOP);
	}
}


// *************************************************************************************************
// @fn          radio_start
// @brief       Prepare radio for RF communication. 
// @param       none
// @return      none
// *************************************************************************************************
void radio_start(void)
{	
	// display logo
	display_symbol(LCD_ICON_BEEPER1, SEG_ON);
	display_symbol(LCD_ICON_BEEPER2, SEG_ON);
	display_symbol(LCD_ICON_BEEPER3, SEG_ON);
	
	// Reset radio core
	radio_reset();
	radio_config();

	// goto receive mode
	radio_receive_on();
}


// *************************************************************************************************
// @fn          radio_stop
// @brief       Shutdown radio for RF communication. 
// @param       none
// @return      none
// *************************************************************************************************
void radio_stop(void)
{
	// Disable radio IRQ
	RF1AIFG = 0;
	RF1AIE  = 0; 
		
	// Reset radio core
	radio_reset();
	
	// Put radio to sleep
	radio_powerdown();
	
	display_symbol(LCD_ICON_BEEPER1, SEG_OFF);
	display_symbol(LCD_ICON_BEEPER2, SEG_OFF);
	display_symbol(LCD_ICON_BEEPER3, SEG_OFF);
}


void radio_transmit(u8 *data, u8 length)
{	
	u8 status = 0;
	
	// get radio into idle state
	status = rf1a_strobe(RF_SIDLE);
	while (status & 0xF0)
	{
		status = rf1a_strobe(RF_SNOP);
	}

	// clear tx fifo
	status = rf1a_strobe(RF_SFTX | RF_TXSTAT);
	while ((status & 0x0F) != 0x0F)
	{
		status = rf1a_strobe(RF_SNOP | RF_TXSTAT);
	}
	
	// write data into fifo
	rf1a_write_tx(data, length);
	
	// enable transmit interrupt
	RF1AIES |= BIT9;                          
	RF1AIFG &= ~BIT9;
	RF1AIE |= BIT9;
	
	// get radio into tx staate
	status = rf1a_strobe(RF_STX);
	while ((status & 0xF0) != 0x20)
	{
		status = rf1a_strobe(RF_SNOP);
	}
}

void radio_receive(u8 *data, u8 length)
{
	// get packet
	rf1a_read_rx(data, length);

	// goto receive mode
	radio_receive_on();
}

void radio_transmitted(void)
{
	request.flag.radio_transmitted = 0;
	// this will clear rx/tx fifo
	radio_receive_on();
}

void radio_received(void)
{
	request.flag.radio_received = 0;
	// this will clear rx/tx fifo
	radio_receive_on();
}

// *************************************************************************************************
// @fn          GDOx_ISR
// @brief       GDO0/2 ISR to detect received packet. 
// @param       none
// @return      none
// *************************************************************************************************
#pragma vector=CC1101_VECTOR
__interrupt void CC1101_ISR(void)
{
	u16 rf1aiv = RF1AIV;
	
	if (rf1aiv == RF1AIV_RFIFG9)
	{
		RF1AIES &= ~BIT9; 
		RF1AIFG &= ~BIT9;
		RF1AIE &= ~BIT9;
		request.flag.radio_transmitted = 1;
	}
	else if (rf1aiv == RF1AIV_RFIFG10)
	{
		RF1AIFG &= ~BITA;
		RF1AIE &= ~BITA;
		request.flag.radio_received = 1;
	}

	// Exit from LPM3/LPM4 on RETI
	__bic_SR_register_on_exit(LPM4_bits); 
}
