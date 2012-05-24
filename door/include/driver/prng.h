#ifndef _DRIVER_PRNG_H_
#define _DRIVER_PRNG_H_

#include "project.h"

extern void prng_seed(u16 seed);
extern void prng_rssi(u8 rssi);
extern void prng_reseed(void);
extern u8 prng_rand(void);

#endif /*_DRIVER_PRNG_H_*/

