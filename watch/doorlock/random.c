// *************************************************************************************************
// Doorlock random bits collection functions.
// *************************************************************************************************

// *************************************************************************************************
// Include section
#include "project.h"

// drivers
#include "driver/vti_as.h"
#include "driver/idle.h"
#include "driver/timer.h"
#include "driver/aes.h"

// doorlock
#include "doorlock/random.h"

// *************************************************************************************************
// Prototypes section
void doorlock_random(u8 random[16]);
void doorlock_random_timer(void);

// *************************************************************************************************
// Global variable section

volatile u8 doorlock_random_index = 0;


// *************************************************************************************************
// @fn          doorlock_random
// @brief       garther random bits using accelerometer
// @param       random bits (output)
// @return      none
// *************************************************************************************************
void doorlock_random(u8 random[16])
{
	u8 longrandom[32] = {0};
	
	// initialize
	doorlock_random_index = 0;
	
	// start accelerometer
	as_start(AS_MODE_2G_100HZ);
	
	// start timer
	fptr_Timer0_A1_function = doorlock_random_timer;
	Timer0_A1_Start(CONV_MS_TO_TICKS(DOORLOCK_RANDOM_INTERVAL));
	
	for(;;)
	{
		idle();
		
		if (doorlock_random_index >= 32)
		{
			as_stop();
			break;
		}
		
		// look for accelerometer data ready event
		if (!request.flag.acceleration_measurement)
		{
			continue;
		}
		
		request.flag.acceleration_measurement = 0;
		
		// "randomly" flipping bits
		longrandom[(doorlock_random_index + 0) % 32] ^= as_get_x();
		longrandom[(doorlock_random_index + 10) % 32] ^= as_get_y();
		longrandom[(doorlock_random_index + 20) % 32] ^= as_get_z();
	}
	
	// just to make sure
	Timer0_A1_Stop();
	
	// now encrypt
	aes_encrypt(longrandom, longrandom + 16);
	
	// done
	memcpy(random, longrandom, sizeof(u8) * 16);
}

// *************************************************************************************************
// @fn          doorlock_random_timer
// @brief       this timer callback changes the index in the array where the random bits are
//				collected to, until the array is filled.
// @param       none
// @return      none
// *************************************************************************************************
void doorlock_random_timer(void)
{
	if (doorlock_random_index < 32)
	{
		++ doorlock_random_index;
	}
	else
	{
		Timer0_A1_Stop();
	}
}
