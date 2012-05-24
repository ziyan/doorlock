// *************************************************************************************************
// Doorlock settings.
// *************************************************************************************************

#ifndef _DOORLOCK_SETTINGS_H_
#define _DOORLOCK_SETTINGS_H_

// radio limits
#define DOORLOCK_RADIO_TRIALS						(12u)
#define DOORLOCK_RADIO_TIMEOUT						(30u)

// sequence limits
#define DOORLOCK_SEQUENCE_MAX_LENGTH				(12u)
#define DOORLOCK_SEQUENCE_MIN_LENGTH				(2u)
#define DOORLOCK_SEQUENCE_PAUSE_RESOLUTION			(32768u/200u)
#define DOORLOCK_SEQUENCE_PAUSE_MAX_LENGTH			(1200u/5u)
#define DOORLOCK_SEQUENCE_PAUSE_MIN_LENGTH			(15u/5u)
#define	DOORLOCK_SEQUENCE_TAP_THRESHOLD				(120)
#define	DOORLOCK_SEQUENCE_TIMEOUT					(30u)

// random bits collection interval
#define DOORLOCK_RANDOM_INTERVAL					(150u)

#endif /*_DOORLOCK_SETTINGS_H_*/
