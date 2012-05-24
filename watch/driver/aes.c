// *************************************************************************************************
// 128-bit AES functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "project.h"

#include "driver/aes.h"

// *************************************************************************************************
// Prototypes section
void aes_encrypt(u8 data[16], u8 key[16]);
void aes_decrypt(u8 data[16], u8 key[16]);

// *************************************************************************************************
// Defines section

// *************************************************************************************************
// Global Variable section

// *************************************************************************************************
// Extern section

// *************************************************************************************************
// @fn          aes_encrypt
// @brief       AES encrypt
// @param       data - 128 bit data to be encrypted in place, key - 128 bit key
// @return      none
// *************************************************************************************************
void aes_encrypt(u8 data[16], u8 key[16])
{
	u8 i = 0;
	
	// software reset
	AESACTL0 |= AESSWRST;
	// disable interrupt
	AESACTL0 &= ~AESRDYIE;

	// set encryption
	AESACTL0 &= ~(AESOP0 | AESOP1);

	// load key
	for (i = 0; i < 16; i++)
	{
		AESAKEY_L = key[i];
	}
	
	// load data
	for (i = 0; i < 16; i++)
	{
		AESADIN_L = data[i];
	}
	
	// wait till complete
	while (AESASTAT & AESBUSY);
	
	// read data
	for (i = 0; i < 16; i++)
	{
		 data[i] = AESADOUT_L;
	}
}

// *************************************************************************************************
// @fn          aes_decrypt
// @brief       AES decrypt
// @param       data - 128 bit data to be decrypted in place, key - 128 bit key
// @return      none
// *************************************************************************************************
void aes_decrypt(u8 data[16], u8 key[16])
{
	u8 i = 0;
	
	// software reset
	AESACTL0 |= AESSWRST;
	// disable interrupt
	AESACTL0 &= ~AESRDYIE;
	
	// set decryption
	AESACTL0 &= ~AESOP1;
	AESACTL0 |= AESOP0;
	
	// load key
	for (i = 0; i < 16; i++)
	{
		AESAKEY_L = key[i];
	}
	
	// load data
	for (i = 0; i < 16; i++)
	{
		AESADIN_L = data[i];
	}
	
	// wait till complete
	while (AESASTAT & AESBUSY);
	
	// read data
	for (i = 0; i < 16; i++)
	{
		 data[i] = AESADOUT_L;
	}
}
