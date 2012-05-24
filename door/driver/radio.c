// *************************************************************************************************
// Radio core access functions.
// *************************************************************************************************


// *************************************************************************************************
// Include section
#include <string.h>
#include "driver/radio.h"
#include "driver/prng.h"
#include "driver/timer.h"
#include "logic/packet.h"

// *************************************************************************************************
// Prototype section
void radio_init(void);
void radio_receive(void);
void radio_receive_again(void);
void radio_retrieve(packet_t *packet);
void radio_transmit(packet_t *packet);
void radio_transmit_again(void);

// *************************************************************************************************
// Global section
__no_init volatile packet_t radio_rx_packet;
__no_init volatile packet_t radio_tx_packet;
volatile u8 radio_rx_index = 0;
volatile u8 radio_tx_index = 0;
volatile u8 radio_tx_repeat = 0;
volatile u8 radio_rx_repeat = 0;

// *************************************************************************************************
// Define section
#define RADIO_TX_REPEAT 10
#define RADIO_RX_REPEAT 2

// *************************************************************************************************
// @fn          radio_init
// @brief       Initialize radio.
// @param       none
// @return      none
// *************************************************************************************************
void radio_init(void)
{
	FSCTRL1   = 0x08;	// Frequency synthesizer control.
	FSCTRL0   = 0x00;	// Frequency synthesizer control.
	FREQ2     = 0x26;	// Frequency control word, high byte.
	FREQ1     = 0x35;	// Frequency control word, middle byte.
	FREQ0     = 0x55;	// Frequency control word, low byte.
	MDMCFG4   = 0xCA;	// Modem configuration.
	MDMCFG3   = 0xA3;	// Modem configuration.
	MDMCFG2   = 0x93;	// Modem configuration.
	MDMCFG1   = 0x23;	// Modem configuration.
	MDMCFG0   = 0x11;	// Modem configuration.
	CHANNR    = 0x30;	// Channel number.
	DEVIATN   = 0x36;	// Modem deviation setting (when FSK modulation is enabled).
	FREND1    = 0x56;	// Front end RX configuration.
	FREND0    = 0x10;	// Front end RX configuration.
	MCSM0     = 0x18;	// Main Radio Control State Machine configuration.
	FOCCFG    = 0x16;	// Frequency Offset Compensation Configuration.
	BSCFG     = 0x6C;	// Bit synchronization Configuration.
	AGCCTRL2  = 0x43;	// AGC control.
	AGCCTRL1  = 0x40;	// AGC control.
	AGCCTRL0  = 0x91;	// AGC control.
	FSCAL3    = 0xE9;	// Frequency synthesizer calibration.
	FSCAL2    = 0x2A;	// Frequency synthesizer calibration.
	FSCAL1    = 0x00;	// Frequency synthesizer calibration.
	FSCAL0    = 0x1F;	// Frequency synthesizer calibration.
	TEST2     = 0x81;	// Various test settings.
	TEST1     = 0x35;	// Various test settings.
	TEST0     = 0x09;	// Various test settings.
	PA_TABLE0 = 0x8E;	// PA output power setting.

	PKTCTRL1  = 0x04;	// Enable append status
	PKTCTRL0  = 0x44;	// Fixed packet length, data whitening enabled
	ADDR      = 0x00;	// Device address.
	PKTLEN    = 0x16;	// Packet length
	MCSM1     = 0x30;	// Go back to IDLE after RX or TX
	MCSM2     = 0x00;

	//
	// Enable interrupts.
	//
	RFIF = 0;
	IEN2 |= IEN2_RFIE;			// General RF interrupts
	RFTXRXIE = 1;				// CPU interrupts for RF TX done / RX ready
	RFIM |= RFIF_IRQ_DONE |		// packet completion
			RFIF_IRQ_TXUNF |	// tx underflow
			RFIF_IRQ_RXOVF |	// rx overflow
			RFIF_IRQ_SFD |		// start frame delimiter
			RFIF_IRQ_TIMEOUT;	// rx timeout

	//
	// Go into idle state.
	//
	RFST = RFST_SIDLE;
	while (MARCSTATE != MARC_STATE_IDLE);
}

void radio_receive(void)
{
	// wait till idle state first
	RFST = RFST_SIDLE;
	while (MARCSTATE != MARC_STATE_IDLE);

	// prepare
	radio_rx_index = 0;
	radio_rx_repeat = RADIO_RX_REPEAT;
	
	// go to rx state
	RFST = RFST_SRX;
	while (MARCSTATE != MARC_STATE_RX);
}

void radio_retrieve(packet_t *packet)
{
	// simply copy the packet out
	memcpy((u8*)packet, (u8*)(&radio_rx_packet), PACKET_RECEIVE_LENGTH);	
}

void radio_transmit(packet_t *packet)
{
	RFST = RFST_SIDLE;

	// prepare to transmit
	memcpy((u8*)(&radio_tx_packet), (u8*)packet, PACKET_TRANSMIT_LENGTH);

	// wait till idle state first
	while (MARCSTATE != MARC_STATE_IDLE);
	
	// prepare to transmit
	radio_tx_index = 0;
	radio_tx_repeat = RADIO_TX_REPEAT;
	
	// go to tx state
	RFST = RFST_STX;
	while (MARCSTATE != MARC_STATE_TX);
}

#pragma vector = RF_VECTOR
__interrupt void radio_rf_ISR(void)
{
	S1CON &= ~(S1CON_RFIF_1 + S1CON_RFIF_0);

	if (RFIF & RFIF_IRQ_DONE)
	{		
		// full packet received or transmitted, go to idle
		if (radio_tx_index == PACKET_TRANSMIT_LENGTH)
		{
			// reset
			RFST = RFST_SIDLE;
			radio_tx_index = 0;

			// done transmitting
			if (!radio_tx_repeat)
			{
				// transmitted enough times
				radio.flag.tx = 1;
			}
			else
			{
				radio_tx_repeat--;
				
				// go to tx state
				while (MARCSTATE != MARC_STATE_IDLE);
				RFST = RFST_STX;
				while (MARCSTATE != MARC_STATE_TX);
			}
		}
		else if (radio_rx_index == PACKET_RECEIVE_LENGTH)
		{
			// reset
			RFST = RFST_SIDLE;
			radio_rx_index = 0;

			// done receiving
			radio.flag.rx = 1;
			
			// seed random number generator using rssi
			if (radio_rx_packet.status.rssi)
				prng_rssi(radio_rx_packet.status.rssi);
		}
		else
		{
			// reset
			RFST = RFST_SIDLE;
			radio_rx_index = 0;
			radio_tx_index = 0;

			// basically this should not happen
			radio.flag.error = 1;
		}
	}

	if (RFIF & RFIF_IRQ_TXUNF)
	{
		// tx underflow, this should not happen, reset
		radio_tx_index = 0;
		RFST = RFST_SIDLE;
		radio.flag.error = 1;
	}
	
	if (RFIF & RFIF_IRQ_RXOVF)
	{
		// rx overflow, this should not happen, reset
		radio_rx_index = 0;
		RFST = RFST_SIDLE;
		radio.flag.error = 1;
	}
	
	if (RFIF & RFIF_IRQ_SFD)
	{
		// Start Frame Delimiter
		// start receiving
		radio_rx_index = 0;
	}
	
	if (RFIF & RFIF_IRQ_TIMEOUT)
	{
		// reset
		RFST = RFST_SIDLE;
		radio_rx_index = 0;

		// rx timeout
		if (!radio_rx_repeat)
		{
			// rx timeout, time to go back to sleep
			radio.flag.timeout = 1;
		}
		else
		{
			radio_rx_repeat--;
			// go to tx state
			while (MARCSTATE != MARC_STATE_IDLE);
			RFST = RFST_SRX;
			while (MARCSTATE != MARC_STATE_RX);
		}
	}
	RFIF = 0;
}

#pragma vector = RFTXRX_VECTOR
__interrupt void radio_rftxrx_ISR(void)
{
	u8 data;

	if (MARCSTATE == MARC_STATE_RX)
	{
		if (radio_rx_index < PACKET_RECEIVE_LENGTH)
		{
			data = RFD;
			((u8*)(&radio_rx_packet))[radio_rx_index] = data;
			++radio_rx_index;
		}
		else
		{
			radio.flag.error = 1;
		}
	}
	else if (MARCSTATE == MARC_STATE_TX)
	{
		if (radio_tx_index < PACKET_TRANSMIT_LENGTH)
		{
			RFD = ((u8*)(&radio_tx_packet))[radio_tx_index];
			++radio_tx_index;
		}
		else
		{
			radio.flag.error = 1;
		}
	}
}
