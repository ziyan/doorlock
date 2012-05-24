#include "driver/prng.h"

void prng_seed(u16 seed);
void prng_rssi(u8 rssi);
void prng_reseed(void);
u8 prng_rand(void);

volatile u16 prng_current_seed = 0;


void prng_seed(u16 seed)
{
	RNDL = seed & 0xFF;
	RNDL = (seed >> 8) & 0xFF;
	
}

void prng_rssi(u8 rssi)
{
	u16 half_seed = prng_current_seed << 8;
	prng_current_seed ^= half_seed;
	prng_current_seed &= 0xFF00u;
	prng_current_seed |= (u16)rssi & 0x00FF;
}

void prng_reseed(void)
{
	prng_seed(prng_current_seed);
}


u8 prng_rand(void)
{
	ADCCON1 |= ADCCON1_RCTRL0;
	while (ADCCON1 & (ADCCON1_RCTRL0 | ADCCON1_RCTRL1));
	return RNDL;
}

